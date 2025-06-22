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

		// empty pipeline
		for (int i = 0; i < pipelineSizePF_float - 1; i++) {
			pipeline[i] = createNOP();
		}

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

			if (INST.target = 0) { // Not a PF_float op
				INST = createNOP();
			}

			out = pipeline[pipelineSizePF_float-1]; // exit pipeline

			// If output instruction is a NOP, check pipeline for completed instruction
			// Increases latency of instructions like FMV (latency = 1)
			if (out.address == 0xffffffff) { // NOP
				for (int i = pipelineSizePF_float-1; i != 0; i--) {
					if (pipeline[i].address != 0xffffffff &&
						getLatencyOp(out.aluOp, out.target) <= i) {
						out = pipeline[i];
						break;
					}
				}
			}

			// Loop to shift pipeline content
			// Pos 0: exit
			// Pos latencyMUL-1: newElement
			for (int i = 0; i < pipelineSizePF_float - 1; i++) {
				pipeline[i] = pipeline[i + 1];
			}

			pipeline[0] = INST; // new instruction

			rs1 = out.I(19, 15);
			rd = out.I(11, 7);

			switch (out.I(31, 27)) {

			case FADDS: 

				rs2 = out.I(24, 20);

				regsFloat[rd] = regsFloat[rs1] + regsFloat[rs2];
				break;

			case FSUBS: 
				rs2 = out.I(24, 20);

				regsFloat[rd] = regsFloat[rs1] - regsFloat[rs2];
				break;

			case FMULS: 
				rs2 = out.I(24, 20);

				regsFloat[rd] = regsFloat[rs1] * regsFloat[rs2];
				break;

			case FCVTWS: // fcvt.w[u].s rd, fs1 R Convert to [un]signed 32-bit integer
				// in decod_registers dataOut will be converted to unsigned if necessary

				out.wReg = true;
				out.dataOut = regsFloat[rs1];
				out.rd = rd;
				strcpy(out.desc, "fcvtws");

				break;

			case FCVTSW: // fcvt.s.w[u] fd, rs1 R Convert from[un]signed 32 - bit integer
				// rs1 - number integer register 
				// opA - value of rs1 (from regs in decod)

				if (out.I(20, 20)) { // unsigned
					regsFloat[rd] = (sc_uint<32>) out.opA;
				}
				else { // signed
					regsFloat[rd] = (sc_int<32>) out.opA;
				}

				out.wReg = false;
				strcpy(out.desc, "fcvtsw");

				break;

			case FMVXS: // fmv.x.s rd, fs1 R Move from floating - point to integer register
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

				out.dataOut = *puntInt;

				//out.dataOut = (int)tmp;

				out.wReg = true;
				out.rd = rd;
				strcpy(out.desc, "fmvxs");

				break;

			case FMVSX: // fmv.s.x fd, rs1 R Move from integer to floating - point register
				// no cast - exact same binary sequence

				regsFloat[rd] = out.opA;
				out.wReg = false;
				strcpy(out.desc, "fmvsx");

				break;

			default:
				out = createNOP();
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
	if (instIn.read().target == 1 && instIn.read().wReg) {

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

