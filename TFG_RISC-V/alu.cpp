#include"alu.h"



void alu::registro(){

	sc_int<32> A, B, res;
	sc_uint<5> opCode;
	sc_uint<5> shamt;
	double tiempo;
	char nem[7]; 

	tiempo = sc_time_stamp().to_double() / 1000.0;

	if( rst.read() ){

		instOut.write(INST); 

	}else{

		INST = I.read(); 

		A = INST.opA;
		B = INST.opB;
		shamt = B(4, 0);
		opCode = INST.aluOp;

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
					res = A >> B ;		 break;
		case SRA: 	strcpy(nem, "sra");
					res = ((sc_int<32>)A) >> B ; break;			// check

		case MUL:	strcpy(nem, "mul");
					res = ((sc_int<32>)A) * ((sc_int<32>)B); break;			// latencia de 1 ciclo -> cambiar por 2 o m�s
		case MULHU:	strcpy(nem, "mul");
			res = A(15, 0) * B(15, 0);
			res = res(31, 16);			 break;			// latencia de 1 ciclo -> cambiar por 2 o m�s

		default: cerr << "Error at the ALU, unknown ALU opcode " << opCode << endl;
					exit(-1);
		};

		if(!strcmp(INST.desc, "ALU"))
			strcpy(INST.desc, nem); 
		else 
			if(!strcmp(INST.desc, "ALUinm")){
				strcpy(INST.desc, nem); 
				strcat(INST.desc, "i");
			}
		INST.aluOut = res; 	

		instOut.write(INST); 

	}


}
