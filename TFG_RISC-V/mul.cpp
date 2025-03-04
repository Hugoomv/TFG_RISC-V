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
		for (int i = 0; i < pipelineSize; i++) {
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
		for (int i = 0; i < pipelineSize - 1; i++) {
			pipeline[i] = pipeline[i + 1];
		}

		// New instruction
		pipeline[pipelineSize - 1] = INST;


		// Operate
		switch (opCode)
		{
		case MUL:
			pipeline[pipelineSize - 1].aluOut = pipeline[pipelineSize -1].dataOut = ((sc_int<32>)A) * ((sc_int<32>)B);
			strcpy(pipeline[pipelineSize - 1].desc, "mul");
			break;

		case MULHU:
			res = A(15, 0) * B(15, 0);
			pipeline[pipelineSize - 1].aluOut = pipeline[pipelineSize - 1].dataOut = res(31, 16);
			strcpy(pipeline[pipelineSize - 1].desc, "mulhu");
			break;

		default:
			pipeline[pipelineSize - 1] = createNOP();
			break;
		}
	} 
}

void mul::hazardDetection() {

	int rs1 = rs1In.read();
	int rs2 = rs2In.read();

	bool aux1 = false, 
		 aux2 = false;

	for (int i = 0; i < pipelineSize; i++) {

		if (pipeline[i].wReg) {

			if (rs1 == pipeline[i].rd) {
				aux1 = true;
			}

			if (rs2 == pipeline[i].rd) {
				aux2 = true;
			}
		}
	}

	hzrdRs1.write(aux1);
	hzrdRs2.write(aux2);

}