#include "mul.h"
#include "alu.h"


// COMPLETELY SEGMENTED
void mul::multiplication() { 

	sc_int<32> A, B, res;
	sc_uint<5> opCode;
	short target;
	double tiempo;

	double pipelineResultsMul[latencyMUL] = {};			// multiplication results
	short pipelineTargetRegisterMul[latencyMUL] = {};	// target = reg
	bool pipelineValidityMul[latencyMUL] = {};			// 1 if value is not thrash

	tiempo = sc_time_stamp().to_double() / 1000.0;

	if (rst.read()) {

		instOut.write(INST); // NOP

		// empty pipeline
		for (int i = 0; i < latencyMUL; i++) {
			pipelineResultsMul[i] = 0;
			pipelineTargetRegisterMul[i] = 0;
			pipelineValidityMul[i] = 0;
		}


	} else {
		
		INST = I.read();

		A = INST.opA;
		B = INST.opB;
		target = INST.rd;
		opCode = INST.aluOp;

		INST.dataOut = INST.aluOut = pipelineResultsMul[0]; // = INST.dataOut ??
		INST.rd = pipelineTargetRegisterMul[0];
		INST.wReg = pipelineValidityMul[0];


		// Loop to shift pipeline content 
		// Pos 0: exit
		// Pos latencyMUL-1: new_element
		for (int i = 0; i < latencyMUL - 1; i++) {
			pipelineResultsMul[i] = pipelineResultsMul[i + 1];
			pipelineTargetRegisterMul[i] = pipelineTargetRegisterMul[i + 1];
			pipelineValidityMul[i] = pipelineValidityMul[i + 1];
		}

		if (opCode == MUL) {

			pipelineResultsMul[latencyMUL - 1] = ((sc_int<32>)A) * ((sc_int<32>)B);
			pipelineTargetRegisterMul[latencyMUL - 1] = target;
			pipelineValidityMul[latencyMUL - 1] = true;
			strcpy(INST.desc, "mul");

		}
		else if (opCode == MULHU){
			res = A(15, 0) * B(15, 0);
			pipelineResultsMul[latencyMUL - 1] = res(31, 16);
			pipelineTargetRegisterMul[latencyMUL - 1] = target;
			pipelineValidityMul[latencyMUL - 1] = true;
			strcpy(INST.desc,"mulhu");
		}
		else{
			pipelineResultsMul[latencyMUL - 1] = 0;
			pipelineTargetRegisterMul[latencyMUL - 1] = target;
			pipelineValidityMul[latencyMUL - 1] = false;
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