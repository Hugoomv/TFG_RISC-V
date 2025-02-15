#ifndef MUL_H
#define MUL_H

#include "systemc.h"
#include "structsRV.h"
#include "config.h"


SC_MODULE(mul) {
public:

	sc_in<bool> clk, rst;
	sc_in<instruction> I;

	sc_out<instruction> instOut;

	void multiplication();

	SC_CTOR(mul) {
		cout << "mul: " << name() << endl;

		// NOP
		INST.address = 0xffffffff;	INST.I = 0x13;	INST.aluOp = 0; INST.memOp = 15;
		INST.rs1 = INST.rs2 = INST.rd = 0x1f; 
		INST.wReg = false;
		INST.opA = INST.opB = INST.val2 = INST.aluOut = INST.dataOut = 0x0000dead;
		strcpy(INST.desc, "???");

		SC_METHOD(multiplication);
		sensitive << clk.pos();

	}

private:

	instruction INST;

};

#endif
