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
		for (int i = 0; i < latencyMUL; i++) {
			pipelineResultsMul[i] = 0;
			pipelineTargetRegisterMul[i] = 0;
			pipelineValidityMul[i] = 0;
		}


	} else {
		
		// Get data
		INST = I.read();

		A = INST.opA;
		B = INST.opB;
		target = INST.rd;
		opCode = INST.aluOp;

		bool flagNOP = false;

		// Prepare output
		INST.dataOut = INST.aluOut = pipelineResultsMul[0]; // = INST.dataOut ??
		INST.rd = pipelineTargetRegisterMul[0];
		INST.wReg = pipelineValidityMul[0];


		// Loop to shift pipeline content 
		// Pos 0: exit
		// Pos latencyMUL-1: newElement
		for (int i = 0; i < latencyMUL - 1; i++) {
			pipelineResultsMul[i] = pipelineResultsMul[i + 1];
			pipelineTargetRegisterMul[i] = pipelineTargetRegisterMul[i + 1];
			pipelineValidityMul[i] = pipelineValidityMul[i + 1];
		}

		switch (opCode)
		{
		case MUL:
			pipelineResultsMul[latencyMUL - 1] = ((sc_int<32>)A) * ((sc_int<32>)B);
			strcpy(INST.desc, "mul");
			break;

		case MULHU:
			res = A(15, 0) * B(15, 0);
			pipelineResultsMul[latencyMUL - 1] = res(31, 16);
			strcpy(INST.desc, "mulhu");
			break;

		default:
			pipelineResultsMul[latencyMUL - 1] = 0;
			pipelineTargetRegisterMul[latencyMUL - 1] = 0;
			pipelineValidityMul[latencyMUL - 1] = false;
			flagNOP = true;
			break;
		}

		if (pipelineValidityMul[0] == false) {
			INST = createNOP(); // Next result not valid
		}
		
		if (!flagNOP) {
			//  Valid instruction for MUL module, pipeline will contain relevant data
			pipelineTargetRegisterMul[latencyMUL - 1] = target;
			pipelineValidityMul[latencyMUL - 1] = true;
		}

		instOut.write(INST);

	} 
	

	// -----------Pointers implementation--------------
	/*
	int p_first = 0;
	int p_last = latencyMUL - 1;

	resultMul.write(pipelineResultsMul[p_first]);
	targetMul.write(pipelineTargetRegisterMul[p_first]);
	validMul.write(pipelineValidityMul[p_first]);

	pipelineResultsMul[p_last] = inputMul.read();				// inputMUL - ??? rev
	pipelineTargetRegisterMul[p_last] = regMul.read();
	pipelineValidityMul[p_last] = validMul.read();

	p_first = (p_first + 1) % latencyMUL;
	p_last = (p_last + 1) % latencyMUL;
	*/
}