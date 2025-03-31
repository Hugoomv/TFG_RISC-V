#include "auxFuncs.h"
#include "alu.h"
#include "config.h"


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

int getLatencyOp(int op) {
	switch (op)
	{
	case MUL:		return latencyMUL;		break;
	case MULH:		return latencyMULH;		break;
	case MULHU:		return latencyMULHU;	break;
	case MULHSU:	return latencyMULHSU;	break;
	case DIV:		return latencyDIV;		break;
	default:		return 1;				break;
	}
}