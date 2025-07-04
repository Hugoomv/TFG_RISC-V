#include "auxFuncs.h"
#include "alu.h"
#include "config.h"
#include "pf_float.h"


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

int getLatencyOp(int op, int target) {
	if (target) {
		switch (op)
		{
		case FADDS:		return latencyFADDS;	break;
		case FSUBS:		return latencyFSUBS;	break;
		case FMULS:		return latencyFMULS;	break;
		case FCVTWS:	return latencyFCVTWS;	break;
		case FCVTSW:	return latencyFCVTSW;	break;
		default:		return 1;				break;
		}
	}
	else {
		switch (op)
		{
		case MUL:		return latencyMUL;		break;
		case MULH:		return latencyMULH;		break;
		case MULHU:		return latencyMULHU;	break;
		case MULHSU:	return latencyMULHSU;	break;
		case DIV:		return latencyDIV;		break;
		case DIVU:		return latencyDIVU;		break;
		case REM:		return latencyREM;		break;
		case REMU:		return latencyREMU;		break;
		default:		return 1;				break;
		}
	}
}

void swapVars(sc_int<32> * a, sc_int<32> * b) {
	(*a) = (*a) + (*b);
	(*b) = (*a) - (*b);
	(*a) = (*a) - (*b);
}


bool isMulModuleOp(int op) {
	switch (op) {
	case MUL:
	case MULH:
	case MULHSU:
	case MULHU:
	case DIV:
	case DIVU:
	case REM:
	case REMU:
		return true;
		break;
	default:
		return false;
		break;
	}
}