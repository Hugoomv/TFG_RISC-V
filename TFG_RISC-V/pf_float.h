#ifndef PF_FLOAT_H
#define PF_FLOAT_H

SC_MODULE(pf_float) {
public:

	sc_in<bool> clk, rst;
	sc_in<instruction> I;


	void pf();

	void registrosFloat();


	SC_CTOR(pf_float) {
		cout << "pf_float: " << name() << endl;

		// NOP
		INST = createNOP();

		SC_METHOD(pf);
		sensitive << clk.pos();

	}

private:
	sc_float<32> regsFloat[32];

};

#endif


