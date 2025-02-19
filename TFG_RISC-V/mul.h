#ifndef MUL_H
#define MUL_H

#include "systemc.h"
#include "structsRV.h"
#include "config.h"
#include "auxFuncs.h"


SC_MODULE(mul) {
public:

	sc_in<bool> clk, rst;
	sc_in<instruction> I;

	sc_out<instruction> instOut;

	void multiplication();

	SC_CTOR(mul) {
		cout << "mul: " << name() << endl;

		// NOP
		INST = createNOP();

		SC_METHOD(multiplication);
		sensitive << clk.pos();

	}

private:

	instruction INST;

	double pipelineResultsMul[latencyMUL] = {};			// multiplication results
	short pipelineTargetRegisterMul[latencyMUL] = {};	// target = reg
	bool pipelineValidityMul[latencyMUL] = {};			// 1 if value is not thrash

};

#endif
