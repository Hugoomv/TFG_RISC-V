#ifndef PF_FLOAT_H
#define PF_FLOAT_H

#include "systemc.h"
#include "structsRV.h"
#include "auxFuncs.h"

SC_MODULE(pf_float) {
public:

	sc_in<bool> clk, rst;
	sc_in<instruction> instIn;


	void pf();

	void registrosFloat();


	SC_CTOR(pf_float) {
		cout << "pf_float: " << name() << endl;

		// NOP
		INST = createNOP();

		SC_METHOD(pf);
		sensitive << clk.pos();

		SC_METHOD(registrosFloat);
		sensitive << clk.pos();

	}

private:
	instruction INST;
	sc_uint<32> I;

	sc_int<32> regsFloat[32]; // rev float

};

#endif


