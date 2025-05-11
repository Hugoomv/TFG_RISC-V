#ifndef CORERISCV_H
#define CORERISCV_H

#include <stdio.h>
#include "systemc.h"
#include "fetch.h"
#include "decod.h"
#include "alu.h"
#include "dataMem.h"
#include "mem.h"
#include "mul.h"
#include "structsRV.h"
#include "pf_float.h"

SC_MODULE(coreRiscV) {
public:

	sc_in<bool> clk, rst;

	void writeInst(int addr, int data) { MEM->writeWord(addr, data); };
	void writeWord(int addr, int data) { MEM->writeWord(addr, data); };
	sc_int<32> readWord(int addr) { return MEM->readWord(addr);	};
	int leeELF(FILE* elf);


	SC_CTOR(coreRiscV) {
		cout << "coreRiscV: " << name() << endl;

		instFetch = new fetch("instFetch");
		instDecod = new decod("instDecod");
		instAlu = new alu("instAlu");
		instDataMem = new dataMem("instDataMem");
		instMul = new mul("instMul");
		instPF_float = new pf_float("instPF_float");


		instFetch->clk(clk);
		instFetch->rst(rst);
		instFetch->PCext(PC_DecodFetch);
		instFetch->hazard(hazard);
		instFetch->bubble(bubble);
		instFetch->instOut(iFD);
		instFetch->PCout(PC_FetchDecod);

		instDecod->clk(clk);
		instDecod->rst(rst);
		instDecod->inst(iFD);
		instDecod->PCin(PC_FetchDecod);

		instDecod->hazard(hazard);
		instDecod->bubble(bubble);

		instDecod->fbEx(iDX);
		instDecod->fbMem(iXM);
		instDecod->fbWB(iMW);
		instDecod->instOut(iDX);
		instDecod->PCout(PC_DecodFetch);

		instAlu->clk(clk);
		instAlu->rst(rst);
		instAlu->I(iDX);
		instAlu->instOut(iXM);
		instAlu->readyFenceAluOut(readyFenceAlu);

		instDataMem->clk(clk);
		instDataMem->rst(rst);
		instDataMem->I(iXM);
		instDataMem->instOut(iMW);
		instDataMem->readyFenceMemOut(readyFenceMem);

		// MUL MODULE
		instMul->clk(clk);
		instMul->rst(rst);
		instMul->I(iDX); // Same entry as ALU
		instMul->instOut(iMul);

		instDecod->fbMul(iMul);

		// Hazard detection MUL
		instDecod->rs1Out(rs1);
		instDecod->rs2Out(rs2);

		instMul->rs1In(rs1);
		instMul->rs2In(rs2);

		instMul->hzrdRs1Out(hzrdRs1);
		instMul->hzrdRs2Out(hzrdRs2);
		instMul->readyFenceMulOut(readyFenceMul);

		instDecod->hzrdRs1In(hzrdRs1);
		instDecod->hzrdRs2In(hzrdRs2);
		instDecod->readyFenceMulIn(readyFenceMul);
		instDecod->readyFenceAluIn(readyFenceAlu);
		instDecod->readyFenceMemIn(readyFenceMem);

		//PF_FLOAT
		instPF_float->clk(clk);
		instPF_float->rst(rst);
		instPF_float->instIn(iDX); // Same entry as ALU



		MEM = new mem; 
		instFetch->MEM = MEM; 
		instDataMem->MEM = MEM;

		instDecod->numInst = &(instFetch->numInst);

	}

//private:
	fetch* instFetch;
	decod* instDecod;
	alu* instAlu;
	dataMem* instDataMem;
	mul* instMul;
	pf_float* instPF_float;

	mem* MEM; 

	sc_signal< sc_uint<32> >	PC_DecodFetch, PC_FetchDecod;
	sc_signal< bool >			hazard, bubble;

	sc_signal < instruction >	iFD, iDX, iXM, iMW, iMul;

	sc_signal < sc_uint<5> >	rs1, rs2;
	sc_signal < bool >			hzrdRs1, hzrdRs2;
	sc_signal < bool >			readyFenceMul,readyFenceAlu,readyFenceMem;

/*	sc_signal< sc_int<32> >		wbValue, opA, opB, rs2_DescodAlu, rs2_AluDataMem;
	sc_signal< sc_uint<5> >		mRegEX, mRegMem, mRegWB;
	sc_signal<bool>				wRegEX, wRegMem, wRegWB;

	sc_signal< sc_uint<4> >		aluOp_DescodAlu, memOp_DescodAlu, memOp_AluDataMem;
	sc_signal< sc_int<32> >		aluOut;	*/

};

#endif