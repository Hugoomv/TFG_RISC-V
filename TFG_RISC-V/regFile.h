#ifndef REGFILE_H
#define REGFILE_H

#include "systemc.h"


/// NO SE UTILIZA
/// SU FUNCIONALIDAD SE IMPLEMENTA EN decod, POR CONVENIENCIA


SC_MODULE(regFile) {
public:

	sc_in<bool> clk;
	sc_in<bool> rst;

	sc_in< sc_uint<5> >  	rs1, rs2, rd;
	sc_in< sc_int<32> >   	rdValue;
	sc_in<bool>				wRreg;
	sc_out< sc_int<32> >	reg1, reg2;


	void readProc1();
	void readProc2();
	void writeProc();

	SC_CTOR(regFile) {
		cout << "regFile: " << name() << endl;

		changed.write(false);
		for (int i = 0; i < 32; ++i)	regs[i] = 0;
		SC_METHOD(readProc1);			
		sensitive << rs1 << changed;
		SC_METHOD(readProc2);
		sensitive << rs2 << changed;
		SC_METHOD(writeProc);
		sensitive << clk.pos();
	}


private:
	sc_int<32> regs[32];
	sc_signal<bool> changed;
};

#endif
