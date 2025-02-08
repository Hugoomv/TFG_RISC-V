#include "mul.h"


// COMPLETELY SEGMENTED
void mul::mul() { 
	
	double pipelineResultsMul[latencyMul];
	short pipelineTargetRegisterMul[latencyMul];
	bool pipelineValidityMul[latencyMul];


	resultMul.write( pipelineResultsMul[0] );
	targetMul.write( pipelineTargetRegisterMul[0] );
	validMul.write(pipelineValidityMul[0]);

	// Loop to shift pipeline content
	for (int i = 0; i < latencyMul-1; i++) { 
		pipelineResultsMul[i] = pipelineResultsMul[i + 1];
		pipelineTargetRegisterMul[i] = pipelineTargetRegisterMul[i + 1];
		pipelineValidityMul[i] = pipelineValidityMul[i + 1];
	}

	pipelineResultsMul[latencyMul - 1] = inputMUL.read();		// inputMUL - ???
	pipelineTargetRegisterMul[latencyMul - 1] = regMUL.read();
	pipelineValidityMul[latencyMul - 1] = validMul.read();

	

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