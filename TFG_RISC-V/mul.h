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

	void multiplication();

	SC_CTOR(mul) {
		cout << "alu: " << name() << endl;

		INST.address = 0xffffffff;	INST.I = 0x13;	INST.aluOp = 0; INST.memOp = 15;
		INST.rs1 = INST.rs2 = INST.rd = 0x1f; 
		INST.wReg = false;
		INST.opA = INST.opB = INST.val2 = INST.aluOut = INST.dataOut = 0x0000dead;
		strcpy(INST.desc, "???");

		SC_METHOD(multiplication);
		sensitive << clk.pos();

	}

private:

	sc_signal<sc_uint<32>> resultMul;
	sc_signal<sc_uint<32>> targetMul;
	sc_signal<sc_uint<32>> regMul;
	sc_signal<sc_uint<32>> inputMul;

	sc_signal<bool> validMul;

	instruction INST;

};


#define MUL 16


#endif
