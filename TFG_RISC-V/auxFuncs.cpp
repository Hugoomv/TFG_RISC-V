#include "auxFuncs.h"


/*
Returns a instruction NOP
*/
instruction createNOP() {
	instruction INST;

	INST.address = 0xffffffff;	INST.I = 0x13;	INST.aluOp = 0; INST.memOp = 15;
	INST.rs1 = INST.rs2 = INST.rd = 0x1f;
	INST.wReg = false;
	INST.opA = INST.opB = INST.val2 = INST.aluOut = INST.dataOut = 0x0000dead;
	strcpy(INST.desc, "NOP");

	return INST;
}
