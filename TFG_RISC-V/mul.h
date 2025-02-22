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


	instruction pipeline[latencyMUL];

};

#define MUL 16
#define MULH 17
#define MULHSU 18
#define MULHU 19
#define DIV 20
#define DIVU 21
#define REM 22
#define REMU 23

#endif
