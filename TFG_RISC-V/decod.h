#ifndef DECOD_H
#define DECOD_H

#include "systemc.h"
#include "structsRV.h"

SC_MODULE(decod) {
public:

	sc_in<bool> clk;
	sc_in<bool> rst;

	sc_in< instruction >   	inst;
	sc_in< sc_uint<32> >   	PCin;
	sc_in< instruction >   	fbEx, fbMem, fbWB, fbMul, fbPF_float;

	sc_in < bool >			hzrdRs1In, hzrdRs2In;
	sc_in < bool >			readyFenceMulIn,readyFenceAluIn,readyFenceMemIn;

	sc_out<instruction>		instOut; 
	sc_out<bool>			hazard, bubble;
	sc_out< sc_uint<32> >	PCout;

	sc_out< sc_uint<5> >	rs1Out, rs2Out;

	void decoding();
	void registros();

	SC_CTOR(decod) {
		cout << "decod: " << name() << endl;
		fire.write(0); 

		INST.address = 0xffffffff;	INST.I = 0x13;	INST.aluOp = 0; INST.memOp = 15; 
		INST.rs1 = INST.rs2 = INST.rd = 0x1f; // 
		INST.wReg = false; 
		INST.opA = INST.opB = INST.val2 = INST.aluOut = INST.dataOut = 0x0000dead;
		strcpy(INST.desc, "???");

		SC_METHOD(decoding);
		sensitive << fire << hzrdRs1In << hzrdRs2In;
		SC_METHOD(registros);
		sensitive << clk.pos();
	}

	unsigned int* numInst; 

	bool idx_rs1;
	bool ixm_rs1;
	bool imw_rs1;
	bool imu_rs1;

	bool idx_rs2;
	bool ixm_rs2;
	bool imw_rs2;
	bool imu_rs2;

private:
	sc_int<32> regs[32];
	sc_signal<bool> fire; 
	instruction		INST; 

	sc_uint<5>  C_rd;
	bool		C_wReg;
	sc_uint<5> 	C_aluOp;
	sc_uint<4> 	C_memOp;
	sc_int<32> 	C_opA, C_opB, C_rs2;

	double tiempo;

};

#endif
