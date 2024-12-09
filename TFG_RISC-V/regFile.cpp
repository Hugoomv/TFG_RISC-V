#include"regFile.h"

/// NO SE UTILIZA
/// SU FUNCIONALIDAD SE IMPLEMENTA EN decod, POR CONVENIENCIA


void regFile::readProc1() {

	sc_uint<5> r;

	r = rs1.read();
	reg1.write(regs[r]);
}

void regFile::readProc2() {

	sc_uint<5> r;

	r = rs2.read();
	reg2.write(regs[r]);
}

void regFile::writeProc() {

	sc_uint<5> r;

	if (rst.read()) {
		for (int i = 0; i < 32; ++i)	regs[i] = 0;
	}
	else {
		if (wRreg.read()) {
			r = rd.read();
			if (r) {
				regs[r] = rdValue.read();		// x0 cannot be changed
				changed.write(!changed.read());	// reduces simulation burden
			}
		}
	}
}
