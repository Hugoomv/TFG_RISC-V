#include "pf_float.h"

void pf_float::registrosFloat() {

	tiempo = sc_time_stamp().to_double() / 1000.0;

	if (rst.read()) {
		for (int i = 0; i < 32; ++i)	regsFloat[i] = 0.0;
	}
	else {
		// REV IMPL
	}
}

}

void pf_float::pf() {
	
	sc_int<32> A, B, res;
	sc_uint<5> opCode;
	short target;
	double tiempo;

	tiempo = sc_time_stamp().to_double() / 1000.0;

	if (rst.read()) {

		// NOP
		INST = createNOP();
		instOut.write(INST);

	}
	else {
		// fmv.x.s rd, fs1 R Move from floating - point to integer register
		// fmv.s.x fd, rs1 R Move from integer to floating - point register

		// fcvt.w[u].s rd, fs1 R Convert to [un]signed 32-bit integer
		// fcvt.s.w[u] fd, rs1 R Convert from[un]signed 32 - bit integer
	}
}