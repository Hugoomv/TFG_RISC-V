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
		for (int i = 0; i < pipelineSizeMUL; i++) {
			pipeline[i] = createNOP();
		}


	} else {
		
		// Get data
		INST = I.read();

		A = INST.opA;
		B = INST.opB;
		target = INST.rd;
		opCode = INST.aluOp;

		// Independant pipeline for each instruction - REV
		int cyclesInPipeline = 0;
		instruction output = pipeline[0];

		for (int i = 0; i < pipelineSizeMUL - 1; i++) {

			cyclesInPipeline = pipelineSizeMUL - i;

			if (pipeline[i].wReg && getLatencyOp(pipeline[i].aluOp) <= cyclesInPipeline) {

				output = pipeline[i];
				pipeline[i] = createNOP();
				break;
			}

		}

		instOut.write(output);


		// Loop to shift pipeline content
		// Pos 0: exit
		// Pos latencyMUL-1: newElement
		for (int i = 0; i < pipelineSizeMUL - 1; i++) {
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

		default:
			INST = createNOP();
			break;
		}

		// New instruction
		pipeline[pipelineSizeMUL - 1] = INST;
	} 
	fire.write(!fire.read());
}

void mul::hazardDetection() {

	int rs1 = rs1In.read();
	int rs2 = rs2In.read();

	bool aux1 = false, 
		 aux2 = false;

	// Prevents RAW
	for (int i = 0; i < pipelineSizeMUL; i++) {

		if (pipeline[i].wReg) {

			if (rs1 == pipeline[i].rd) {
				aux1 = true;
			}

			if (rs2 == pipeline[i].rd) {
				aux2 = true;
			}
		}
	}

	// REV - Ben aqui??
	if (instOut.read().wReg) {

		if (rs1 == instOut.read().rd) {
			aux1 = true;
		}

		if (rs2 == instOut.read().rd) {
			aux2 = true;
		}
	}

	
	// REV
	if (pipelineSizeMUL == 2 && I.read().wReg) {

		int opCode = I.read().aluOp;
		if (opCode == 16 || opCode == 17 || opCode == 18 || opCode == 19) {
			aux1 = true;
			aux2 = true;
		}
	}

	hzrdRs1.write(aux1);
	hzrdRs2.write(aux2);
}