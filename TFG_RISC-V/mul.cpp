#include "mul.h"


// COMPLETELY SEGMENTED
void mul::multiplication() { 

	sc_uint<5> opCode;
	double tiempo;
	char nem[7];

	tiempo = sc_time_stamp().to_double() / 1000.0;

	if (rst.read()) {

		instOut.write(INST);

	} else {
		
		INST = I.read();
		opCode = INST.aluOp;
		
		if (opCode == MUL) {

			strcpy(nem, "mul");

			double pipelineResultsMul[latencyMul];
			short pipelineTargetRegisterMul[latencyMul];
			bool pipelineValidityMul[latencyMul];


			resultMul.write(pipelineResultsMul[0]);
			targetMul.write(pipelineTargetRegisterMul[0]);
			validMul.write(pipelineValidityMul[0]);

			// Loop to shift pipeline content
			for (int i = 0; i < latencyMul - 1; i++) {
				pipelineResultsMul[i] = pipelineResultsMul[i + 1];
				pipelineTargetRegisterMul[i] = pipelineTargetRegisterMul[i + 1];
				pipelineValidityMul[i] = pipelineValidityMul[i + 1];
			}

			pipelineResultsMul[latencyMul - 1] = inputMul.read();		// inputMUL - ???
			pipelineTargetRegisterMul[latencyMul - 1] = regMul.read();
			pipelineValidityMul[latencyMul - 1] = validMul.read();

			if (!strcmp(INST.desc, "ALU"))
				strcpy(INST.desc, nem);
			else
				if (!strcmp(INST.desc, "ALUinm")) {
					strcpy(INST.desc, nem);
					strcat(INST.desc, "i");
				}
			INST.aluOut = resultMul.read();

			instOut.write(INST);

		} else {
			// Do nothing - no mul
		}

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