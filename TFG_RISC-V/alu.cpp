#include"alu.h"
#include "auxFuncs.h"



void alu::registro(){

	sc_int<32> A, B, res;
	sc_uint<5> opCode;
	sc_uint<5> shamt;
	double tiempo;
	char nem[7]; 

	bool flagNOP = false;

	tiempo = sc_time_stamp().to_double() / 1000.0;

	if( rst.read() ){

		instOut.write(INST); 

	}else{

		INST = I.read(); 

		A = INST.opA;
		B = INST.opB;
		shamt = B(4, 0);
		opCode = INST.aluOp;

		//if (opCode == ADD && (A < 0 || B < 0) && INST.rd == 10 && INST.wReg) 
			//printf("Alu \t%d - %d(%s): %d[%d] op %d[%d] =>_[%d] --MemOp = %d\n", (int)INST.wReg, (int)INST.aluOp, INST.desc, (int)INST.opA, (int)INST.rs1, (int)INST.opB, (int)INST.rs2, (int)INST.rd,(int)INST.memOp);
		

		switch(opCode){
		case ADD: 
					strcpy(nem, "add");
					res = A + B;		break;
		case SUB:	strcpy(nem, "sub");
					res = A - B;		 break;
		case AND: 	strcpy(nem, "and");
					res = A & B;		break;
		case OR: 	strcpy(nem, "or");
					res = A | B;		 break;
		case XOR: 	strcpy(nem, "xor");
					res = A ^ B;		break;
		case SLT: 	strcpy(nem, "slt");
					res = A<B ? 1 : 0; break;
		case SLTU: 	strcpy(nem, "sltu");
					res = ((sc_int<32>)A)<((sc_int<32>)B) ? 1 : 0 ; break;	// check that works...
		case SLL: 	strcpy(nem, "sll");
					res = A << B ;		 break;
		case SRL: 	strcpy(nem, "srl");
					res = ((sc_uint<32>)A) >> B ;		 break;
		case SRA: 	strcpy(nem, "sra");
					res = ((sc_int<32>)A) >> B ; break;			// check

		case MUL:	flagNOP = true; // Does nothing - NOP 
					break;			
		case MULHU:	flagNOP = true; // Does nothing - NOP 	 
					break;

		default: cerr << "Error at the ALU, unknown ALU opcode " << opCode << endl;
					exit(-1);
		};

		if (flagNOP) {
			INST = createNOP();
		}
		else {

			if (!strcmp(INST.desc, "ALU"))
				strcpy(INST.desc, nem);
			else
				if (!strcmp(INST.desc, "ALUinm")) {
					strcpy(INST.desc, nem);
					strcat(INST.desc, "i");
				}
			INST.aluOut = res;
		}
		

		instOut.write(INST); 

		//if (INST.wReg && (int)INST.aluOut < 0 && (int)INST.rd == 10)
			//printf("Alu \t%d - %d(%s): %d[%d] op %d[%d] =>%d[%d]\n", (int)INST.wReg, (int)INST.aluOp, INST.desc, (int)INST.opA, (int)INST.rs1, (int)INST.opB, (int)INST.rs2, (int)INST.aluOut, (int)INST.rd);


	}
}
