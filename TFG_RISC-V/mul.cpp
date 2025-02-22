#include "mul.h"
#include "alu.h"


// COMPLETELY SEGMENTED
void mul::multiplication() { 

	sc_int<32> A, B, res;
	sc_uint<5> opCode;
	short target;
	double tiempo;

	tiempo = sc_time_stamp().to_double() / 1000.0;

	if (rst.read()) {

		// NOP
		INST = createNOP();
		instOut.write(INST); 

		// empty pipeline
		for (int i = 0; i < latencyMUL; i++) {
			pipeline[i] = createNOP();
		}


	} else {
		
		// Get data
		INST = I.read();

		A = INST.opA;
		B = INST.opB;
		target = INST.rd;
		opCode = INST.aluOp;

		// Output
		instOut.write(pipeline[0]);

		
		// Loop to shift pipeline content 
		// Pos 0: exit
		// Pos latencyMUL-1: newElement
		for (int i = 0; i < latencyMUL - 1; i++) {
			pipeline[i] = pipeline[i + 1];
		}

		// New instruction
		pipeline[latencyMUL - 1] = INST;


		// Operate
		switch (opCode)
		{
		case MUL:
			pipeline[latencyMUL - 1].aluOut = pipeline[latencyMUL -1].dataOut = ((sc_int<32>)A) * ((sc_int<32>)B);
			strcpy(pipeline[latencyMUL - 1].desc, "mul");
			break;

		case MULHU:
			res = A(15, 0) * B(15, 0);
			pipeline[latencyMUL - 1].aluOut = pipeline[latencyMUL - 1].dataOut = res(31, 16);
			strcpy(pipeline[latencyMUL - 1].desc, "mulhu");
			break;

		default:
			pipeline[latencyMUL - 1] = createNOP();
			break;
		}
	} 
}