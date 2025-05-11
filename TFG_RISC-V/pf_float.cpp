#include "pf_float.h"

void pf_float::registrosFloat() {

	double tiempo = sc_time_stamp().to_double() / 1000.0;

	if (rst.read()) {
		for (int i = 0; i < 32; ++i)	regsFloat[i] = 0.0;
	}
	else {
		// REV IMPL
	}
}


void pf_float::pf() {
	
	int rs1, rd;
	double tiempo;

	instruction out;

	tiempo = sc_time_stamp().to_double() / 1000.0;

	INST = instIn.read();

	I = INST.I;

	if (rst.read()) {

		// NOP
		instOut = createNOP();

	}
	else {
		// fmv.x.s rd, fs1 R Move from floating - point to integer register
		// fmv.s.x fd, rs1 R Move from integer to floating - point register

		// fcvt.w[u].s rd, fs1 R Convert to [un]signed 32-bit integer
		// fcvt.s.w[u] fd, rs1 R Convert from[un]signed 32 - bit integer

		rs1 = I(19, 15);
		rd = I(11,7);

		out = INST;

		switch (I(31, 27)) {

		case 24: // fcvt.w[u].s
			// in decod_registers dataOut will be converted to unsigned if necessary
			
			out.wReg = true;
			out.dataOut = regsFloat[rs1];
			out.rd = rd;
			break;

		case 26: // fcvt.s.w[u] - fcvt.s.x REV
			// rs1 - number integer register 
			// opA - value of rs1 (from regs in decod)

			if (I(20, 20)) { // unsigned
				regsFloat[rd] = (sc_uint<32>) INST.opA;
			}
			else { // signed
				regsFloat[rd] = (sc_int<32>) INST.opA;
			}

			out.wReg = false;
			break;

		case 28: // fmv.x.s 
			// no cast - exact same binary sequence

			out.dataOut = regsFloat[rs1];
			out.wReg = true;
			out.rd = rd;

			break;

		case 30: // fmv.s.x
			// no cast - exact same binary sequence

			regsFloat[rd] = INST.opA;
			out.wReg = false;
			break;

		default:
			break;
		}
	}


	instOut.write(out);
}