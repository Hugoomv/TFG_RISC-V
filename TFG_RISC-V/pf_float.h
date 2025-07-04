#ifndef PF_FLOAT_H
#define PF_FLOAT_H

#include "systemc.h"
#include "structsRV.h"
#include "auxFuncs.h"
#include "config.h"

SC_MODULE(pf_float) {
public:

	sc_in<bool> clk, rst;
	sc_in<instruction> instIn, dataMemIn;
	sc_in< sc_uint<5> > rs1In, rs2In;

	sc_out<instruction> instOut;
	sc_out<bool> hzrdRs1Out, hzrdRs2Out;


	void pf();
	void registersFloat();
	void hzrdDetection();


	SC_CTOR(pf_float) {
		cout << "pf_float: " << name() << endl;

		// NOP
		INST = createNOP();

		SC_METHOD(pf);
		sensitive << clk.pos();

		SC_METHOD(registersFloat);
		sensitive << clk.pos();

		SC_METHOD(hzrdDetection);
		sensitive << rs1In << rs2In << fire;

		fire.write(true);

	}

private:
	instruction INST;
	sc_uint<32> I;
	sc_signal < bool > fire;

	bool flagEmpty = false;
	float regsFloat[32]; 

	instruction pipeline[pipelineSizePF_float];

};

#define FADDS	0
#define FSUBS	1
#define FMULS	2
#define FEQS	20
#define FCVTWS	24
#define FCVTSW	26
#define FMVXS	28
#define FMVSX	30

#endif


