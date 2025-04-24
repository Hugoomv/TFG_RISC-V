#include "mul.h"
#include "alu.h"


// COMPLETELY SEGMENTED
void mul::multiplication() { 

	sc_int<32> A, B, res;
	sc_uint<5> opCode;
	short target;
	double tiempo;

	tiempo = sc_time_stamp().to_double() / 1000.0;

	if (rst.read()) {

		// NOP
		INST = createNOP();
		instOut.write(INST); 

		// empty pipeline
		for (int i = 0; i < pipelineSizeMul; i++) {
			pipeline[i] = createNOP();
		}


	} else {
		
		// Get data
		INST = I.read();

		A = INST.opA;
		B = INST.opB;
		target = INST.rd;
		opCode = INST.aluOp;

		// Independant pipeline for each instruction 
		int cyclesInPipeline = 0;
		instruction output = pipeline[0];

		for (int i = 0; i < pipelineSizeMul - 1; i++) {

			cyclesInPipeline = pipelineSizeMul - i;

			if (pipeline[i].wReg && getLatencyOp(pipeline[i].aluOp) <= cyclesInPipeline) {

				output = pipeline[i];
				pipeline[i] = createNOP();

				// Div in output
				if (pipeline[i].aluOp == DIV || pipeline[i].aluOp == DIVU || 
					pipeline[i].aluOp == REM || pipeline[i].aluOp == REMU ) {
					flagDiv = false;
					pipelineFull = false;
				}
				break;
			}
		}

		if (output.aluOp != 0) {
			pipelineFull = false;
		}

		instOut.write(output);

		// Loop to shift pipeline content
		// Pos 0: exit
		// Pos latencyMUL-1: newElement
		for (int i = 0; i < pipelineSizeMul - 1; i++) {
			pipeline[i] = pipeline[i + 1];
		}

		sc_int<64> tmp = 0;

		// Operate
		switch (opCode)
		{
		case MUL: 
			tmp = ((sc_int<32>)A) * ((sc_int<32>)B);
			INST.aluOut = INST.dataOut = tmp(31,0);
			strcpy(INST.desc, "mul");
			break;

		case MULH: 
			tmp = ((sc_int<32>)A) * ((sc_int<32>)B);
			INST.aluOut = INST.dataOut = tmp(63,32);
			strcpy(INST.desc, "mulh");
			break;

		case MULHU:
			tmp = ((sc_uint<32>)A) * ((sc_uint<32>)B);
			INST.aluOut = INST.dataOut = tmp(63, 32);
			strcpy(INST.desc, "mulhu");
			break;

		case MULHSU:
			tmp = ((sc_int<32>)A) * ((sc_uint<32>)B);
			INST.aluOut = INST.dataOut = tmp(63, 32);
			strcpy(INST.desc, "mulhsu");
			break;

		case DIV:
			if (B == 0) {
				cerr << "Divider can't be 0 " << endl;
			}
			INST.aluOut = INST.dataOut = ((sc_int<32>)A) / ((sc_int<32>)B);
			strcpy(INST.desc, "div");
			flagDiv = true;
			break;

		case DIVU:
			if (B == 0) {
				cerr << "Divider can't be 0 " << endl;
			}
			INST.aluOut = INST.dataOut =((sc_uint<32>)A) / ((sc_uint<32>)B);
			strcpy(INST.desc, "divu");
			flagDiv = true;
			break;

		case REM:
			if (B == 0) {
				cerr << "Divider can't be 0 " << endl;
			}
			INST.aluOut = INST.dataOut = ((sc_int<32>)A) % ((sc_int<32>)B);
			strcpy(INST.desc, "rem");
			flagDiv = true;
			break;

		case REMU:
			if (B == 0) {
				cerr << "Divider can't be 0 " << endl;
			}
			INST.aluOut = INST.dataOut =  ((sc_uint<32>)A) % ((sc_uint<32>)B);
			strcpy(INST.desc, "remu");
			flagDiv = true;
			break;

		default:
			INST = createNOP();
			break;
		}

		// New instruction
		pipeline[pipelineSizeMul - 1] = INST;

	} 
	fire.write(!fire.read());
}

void mul::hazardDetection() {

	int rs1 = rs1In.read();
	int rs2 = rs2In.read();


	bool aux1 = false, 
		 aux2 = false;

	int cont = 0;
	bool emptyPipeline = false;

	// Prevents RAW
	for (int i = 0; i < pipelineSizeMul; i++) {

		if (pipeline[i].wReg) {

			if (rs1 == pipeline[i].rd) {
				aux1 = true;
			}

			if (rs2 == pipeline[i].rd) {
				aux2 = true;
			}
		}
		else {
			cont++;
		}
	}

	if (instOut.read().wReg) {

		if (rs1 == instOut.read().rd) {
			aux1 = true;
		}

		if (rs2 == instOut.read().rd) {
			aux2 = true;
		}
	}
	else {
		emptyPipeline = true;
	}

#if SOLO_1OP

	if (I.read().wReg) {
		int opCode = I.read().aluOp;

		if (isMulModuleOp(opCode)) {
			aux1 = aux2 = true;
			pipelineFull = true;
		}
		else if (!pipelineFull) {
			aux1 = aux2 = false;
		}
	}
	else {
		aux1 = aux2 = true;
		emptyPipeline = emptyPipeline && true;
	}
	
	
#else

	if (I.read().wReg) {
		int opCode = I.read().aluOp;

		if (flagDiv && isMulModuleOp(opCode)) {
			if (opCode == DIV || opCode == DIVU || opCode == REM || opCode == REMU) {
				aux1 = aux2 = true;
			}
			else {
				aux1 = aux2 = false;
			}
		}
	}
	else {
		aux1 = aux2 = false;
		emptyPipeline = emptyPipeline && true;
	}
#endif

	if (cont == pipelineSizeMul && emptyPipeline) {
		readyFenceMulOut.write(true);
	}
	else {
		readyFenceMulOut.write(false);
	}

	hzrdRs1Out.write(aux1);
	hzrdRs2Out.write(aux2);
}