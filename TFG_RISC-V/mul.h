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

	// Hazard detection from Decod
	sc_in< sc_uint<5> > rs1In, rs2In;
	sc_out< bool > hzrdRs1, hzrdRs2;

	sc_out<instruction> instOut;


	void multiplication();

	void hazardDetection();

	SC_CTOR(mul) {
		cout << "mul: " << name() << endl;

		// NOP
		INST = createNOP();

		SC_METHOD(multiplication);
		sensitive << clk.pos();

		SC_METHOD(hazardDetection);
		sensitive << rs1In << rs2In << fire;

		fire.write(true);

	}

private:

	instruction INST;

	sc_signal < bool > fire;

	instruction pipeline[pipelineSize];

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
