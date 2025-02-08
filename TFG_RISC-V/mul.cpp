#include "mul.h"


// COMPLETELY SEGMENTED
void mul::multiplication() { 

	sc_int<32> A, B;
	sc_uint<5> opCode;
	double tiempo;

	tiempo = sc_time_stamp().to_double() / 1000.0;

	if (rst.read()) {

		instOut.write(INST);

	} else {
		
		INST = I.read();

		A = INST.opA;
		B = INST.opB;
		opCode = INST.aluOp;

		double pipelineResultsMul[latencyMul] = {};
		//short pipelineTargetRegisterMul[latencyMul];
		bool pipelineValidityMul[latencyMul] = {};

		resultMul.write(pipelineResultsMul[0]);
		//targetMul.write(pipelineTargetRegisterMul[0]); // REV??
		validMul.write(pipelineValidityMul[0]);

		// Loop to shift pipeline content 
		// Pos 0: exit
		// Pos latencyMul-1: new_element
		for (int i = 0; i < latencyMul - 1; i++) {
			pipelineResultsMul[i] = pipelineResultsMul[i + 1];
			//pipelineTargetRegisterMul[i] = pipelineTargetRegisterMul[i + 1];
			pipelineValidityMul[i] = pipelineValidityMul[i + 1];
		}

		pipelineResultsMul[latencyMul - 1] = ((sc_int<32>)A) * ((sc_int<32>)B);
		//pipelineTargetRegisterMul[latencyMul - 1] = regMul.read();
		pipelineValidityMul[latencyMul - 1] = true;

		strcpy(INST.desc, "mul");
		INST.aluOut = resultMul.read();

		instOut.write(INST);

	} 
	

	// -----------Pointers implementation--------------
	/*
	int p_first = 0;
	int p_last = latencyMul - 1;

	resultMul.write(pipelineResultsMul[p_first]);
	targetMul.write(pipelineTargetRegisterMul[p_first]);
	validMul.write(pipelineValidityMul[p_first]);

	pipelineResultsMul[p_last] = inputMul.read();				// inputMUL - ??? rev
	pipelineTargetRegisterMul[p_last] = regMul.read();
	pipelineValidityMul[p_last] = validMul.read();

	p_first = (p_first + 1) % latencyMul;
	p_last = (p_last + 1) % latencyMul;
	*/
}