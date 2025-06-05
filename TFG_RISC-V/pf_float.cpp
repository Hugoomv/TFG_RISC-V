#include "pf_float.h"
#include <string.h>

void pf_float::registersFloat() {

	double tiempo = sc_time_stamp().to_double() / 1000.0;

	if (rst.read()) {
		for (int i = 0; i < 32; ++i)	regsFloat[i] = 0.0;
	}
	else {
		// 
	}
}


void pf_float::pf() {
	
	int rs1, rd, rs2;
	double tiempo;

	instruction out;

	tiempo = sc_time_stamp().to_double() / 1000.0;

	INST = instIn.read();

	I = INST.I;

	if (rst.read()) {

		// NOP
		out = createNOP();

	}
	else {

		if (dataMemIn.read().target) { // FLW from Mem
			rd = I(11, 7);

			regsFloat[rd] = INST.dataOut; 

		}
		else if (decodIn.read().target) { // FSW from Decod
			
			rs2 = I(24, 20); 

			out.val2 = regsFloat[rs2];
			
		}
		else { // Normal PF ops

			rs1 = I(19, 15);
			rd = I(11, 7);

			out = INST;

			switch (I(31, 27)) {

			case 0: // fadd.s
				rs2 = I(24, 20);

				regsFloat[rd] = regsFloat[rs1] + regsFloat[rs2];
				break;

			case 1: // fsub.s
				rs2 = I(24, 20);

				regsFloat[rd] = regsFloat[rs1] - regsFloat[rs2];
				break;

			case 2: // fmul.s
				rs2 = I(24, 20);

				regsFloat[rd] = regsFloat[rs1] * regsFloat[rs2];
				break;

			case 24: // fcvt.w[u].s rd, fs1 R Convert to [un]signed 32-bit integer
				// in decod_registers dataOut will be converted to unsigned if necessary

				out.wReg = true;
				out.dataOut = regsFloat[rs1];
				out.rd = rd;
				strcpy(out.desc, "fcvtws");

				break;

			case 26: // fcvt.s.w[u] fd, rs1 R Convert from[un]signed 32 - bit integer
				// rs1 - number integer register 
				// opA - value of rs1 (from regs in decod)

				if (I(20, 20)) { // unsigned
					regsFloat[rd] = (sc_uint<32>) INST.opA;
				}
				else { // signed
					regsFloat[rd] = (sc_int<32>) INST.opA;
				}

				out.wReg = false;
				strcpy(out.desc, "fcvtsw");

				break;

			case 28: // fmv.x.s rd, fs1 R Move from floating - point to integer register
				// no cast - exact same binary sequence

				/*
				union {
					int i;
					float f;
				} A;

				A.f = regsFloat[rs1];
				out.dataOut = A.i;*/

				int* puntInt;
				float tmp;

				puntInt = (int*)(&tmp);

				tmp = regsFloat[rs1];

				//out.dataOut = *puntInt;

				out.dataOut = (int)tmp;

				out.wReg = true;
				out.rd = rd;
				strcpy(out.desc, "fmvxs");

				break;

			case 30: // fmv.s.x fd, rs1 R Move from integer to floating - point register
				// no cast - exact same binary sequence

				regsFloat[rd] = INST.opA;
				out.wReg = false;
				strcpy(out.desc, "fmvsx");

				break;

			default:
				break;
			}
		}
	}

	fire.write(!fire.read());

	instOut.write(out);
}


void pf_float::hzrdDetection() {

	int rs1 = rs1In.read();
	int rs2 = rs2In.read();

	bool aux1 = false,
		aux2 = false;

	// Input instruction
	if (instIn.read().wReg) { 

		if (rs1 == instIn.read().rd) {
			aux1 = true;
		}

		if (rs2 == instIn.read().rd) {
			aux2 = true;
		}
	}

	// Exit instruction
	if (instOut.read().memOp != 15) { // Not a NOP

		if (rs1 == instOut.read().rd) {
			aux1 = true;
		}

		if (rs2 == instOut.read().rd) {
			aux2 = true;
		}
	}

	hzrdRs1Out.write(aux1);
	hzrdRs2Out.write(aux2);
}

