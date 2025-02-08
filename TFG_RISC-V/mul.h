#ifndef MUL_H
#define MUL_H

#include "systemc.h"
#include "structsRV.h"


#define latencyMul 3


SC_MODULE(mul) {
public:

	sc_in<bool> clk, rst;
	sc_in<instruction> I;

	sc_out<instruction> instOut;

	void mul();

	SC_CTOR(mul) {
		cout << "alu: " << name() << endl;



		SC_METHOD(mul);
		sensitive << clk.pos();

	}

private:

	regMul;
	valMul;
	resultMul;
	targetMul;
	validMul;

};


#endif
