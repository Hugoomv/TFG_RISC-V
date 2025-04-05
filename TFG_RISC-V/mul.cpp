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
		int cyclesRemaining = 0;
		instruction output = pipeline[0];

		for (int i = 0; i < pipelineSizeMul - 1; i++) {

			cyclesRemaining = pipelineSizeMul - i;

			if (pipeline[i].wReg && getLatencyOp(pipeline[i].aluOp) <= cyclesRemaining) {

				output = pipeline[i];
				pipeline[i] = createNOP();

				// Sale una div
				if (pipeline[i].aluOp == 20) {
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
			// excepcion b = 0
			INST.aluOut = INST.dataOut = (B == 0) ? 0 : ((sc_int<32>)A) / ((sc_int<32>)B);
			strcpy(INST.desc, "div");
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

	// Prevents RAW
	for (int i = 0; i < pipelineSizeMul; i++) {// rev pipelineSizeMul-1

		if (pipeline[i].wReg) {

			if (rs1 == pipeline[i].rd) {
				aux1 = true;
			}

			if (rs2 == pipeline[i].rd) {
				aux2 = true;
			}
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

#if SOLO_1OP
	int opCode = I.read().aluOp;
	if (opCode == 16 || opCode == 17 || opCode == 18 || opCode == 19 || opCode == 20) {
		aux1 = aux2 = true;
		pipelineFull = true;
	}
	else if (!pipelineFull) {
		aux1 = aux2 = false;
	}
	else {
		aux1 = aux2 = true;
	}
	
#else

	int opCode = I.read().aluOp;

	if (flagDiv && (opCode == 16 || opCode == 17 || opCode == 18 || opCode == 19)) {
		aux1 = aux2 = true;
	}
	else if (opCode == 16 || opCode == 17 || opCode == 18 || opCode == 19 || opCode == 20) {
		for (int i = 0; i < pipelineSizeMul; i++) {
			if (pipeline[i].aluOp == 20 || pipeline[i].aluOp == 19) {
				aux1 = aux2 = true;
				break;
			}
		}
	}
#endif

	hzrdRs1.write(aux1);
	hzrdRs2.write(aux2);
}