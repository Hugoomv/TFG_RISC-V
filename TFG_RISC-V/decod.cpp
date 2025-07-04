#include "decod.h"
#include "alu.h"			// alu opCodes are defined there
#include "config.h"
#include "auxFuncs.h"


void decod::registros(){		// este método implementa el banco de registros

	instruction backInst; 

	tiempo = sc_time_stamp().to_double() / 1000.0;		// ayuda a depurar, nos dice en qué ciclo estamos
	if (rst.read()) {
		for(int i=0; i<32; ++i)	regs[i] = 0;
	}else{

		// MUL 
		backInst = fbMul.read();

		if (backInst.wReg) {
			int target = backInst.rd;

			if (target) {
				regs[target] = backInst.dataOut;
			}
		} 

		// WB
		backInst = fbWB.read();

		if (backInst.wReg) {
			int target = backInst.rd;

			if (target) {
				regs[target] = backInst.dataOut;
			}
		}

		// PF_float
		backInst = fbPF_float.read();

		if (backInst.wReg) {
			int target = backInst.rd;

			if (target) {
				
				if (backInst.I(20, 20)) {
					regs[target] = (uint32_t)backInst.dataOut;
				}
				else {
					regs[target] = backInst.dataOut;
				}
			}
		}

		
		
		INST.rd = C_rd;			INST.wReg = C_wReg;
		INST.opA = C_opA;		INST.opB = C_opB;
		INST.val2 = C_rs2;
		INST.aluOp = C_aluOp;	INST.memOp = C_memOp;
	}

	instOut.write(INST);
	fire.write( !fire.read());

}

void decod::decoding() {

	sc_uint<32>   	I, PCbranch;
	sc_int<32>   	rs1, rs2;		// signed
	sc_uint<5>		opCode;
	sc_uint<5>		hzRF, hzEX, hzWB;
	sc_uint<5>		regD; 
	sc_int<12>		inm12;
	sc_int<13>		inmBR;
	sc_uint<5>		preAlu;
	sc_uint<4>		preMem;
	sc_int<21>		jalOffset;		// signed
	bool			uRs1, uRs2, hRs1, hRs2, jump, preWrite, flagFence = false;
	double tiempo;

	int rd = 0, csr = 0;

	tiempo = sc_time_stamp().to_double() / 1000.0;

	INST = inst.read();

	I = INST.I;

	INST.target = 0; 
  
  // Assumes this, not correct in all instructions
	INST.rs1 = I(19, 15);
	INST.rs2 = I(24, 20);
	rs1 = regs[I(19, 15)];
	rs2 = regs[I(24, 20)];

	// Hazard Detection with MUL and PF_float module
	rs1Out.write(INST.rs1);
	rs2Out.write(INST.rs2);

	jump = false; 
	C_rs2 = rs2;
	preMem = 15; // nopMem

	opCode = I(6, 2);
	uRs1 = uRs2 = false;
	switch (opCode) {
	case 0:		// Loads
		strcpy(INST.desc, "load"); 
		inm12 = I(31, 20);
		C_opA = rs1;
		C_opB = inm12;
		C_rd = I(11, 7);	preWrite = true;
		preAlu = ADD;			
		preMem = I(14, 12);		
		uRs1 = true; 

		break;

	case 8:		// Stores
		strcpy(INST.desc, "store"); 

		inm12(11, 5) = I(31, 25);	inm12(4, 0) = I(11, 7);
		C_opA = (rs1);
		C_opB = (inm12);
		C_rd = (I(11, 7));		preWrite = false;

		preAlu = ADD;		
		preMem = I(14, 12) | 8;		
		uRs1 = true;	uRs2 = true;
		break;

	case 4: //	Arithmetic with inmediate
		if(I==0x13){
			strcpy(INST.desc, "nop");
		}
		else {
			strcpy(INST.desc, "ALUinm");
		}

		inm12 = I(31, 20);

		C_opA = (rs1);
		C_opB = (inm12);
		C_rd = (I(11, 7));		preWrite = true;
		preAlu.bit(4) = 0;
		// SRLI, SLLI and SRAI
		if ((I(14, 12) == 1) || (I(14, 12) == 5))
			preAlu.bit(3) = I.bit(30);
		else 
			preAlu.bit(3) = 0;
		preAlu(2, 0) = I(14, 12);
		uRs1 = true;
		break;
	case 12: //	Arithmetic with registers
		
		strcpy(INST.desc, "ALU");
		
		C_opA = (rs1);
		C_opB  = (rs2);
		C_rd  = (I(11, 7));		preWrite = true;
		preAlu.bit(4) = I.bit(25);	preAlu.bit(3) = I.bit(30);		preAlu(2, 0) = I(14, 12);	// supports M-extensin
		uRs1 = true;	uRs2 = true;
		break;

	case 13: // LUI
		strcpy(INST.desc, "lui"); 
		C_opA = (0);
		C_opB = (I(31, 12) << 12);
		C_rd = (I(11, 7));		preWrite = true;
		preAlu = OR;		
		break;

	case 5: // AUIPC
		strcpy(INST.desc, "auipc"); 
		C_opA = ((sc_int<32>)PCin.read());
		C_opB = (I(31, 12) << 12);
		C_rd = (I(11, 7));		preWrite = true;
		preAlu = ADD;
		break;

	case 27: // JAL

		strcpy(INST.desc, "jal"); 

//	puede ayudar a depurar	cout << INST << endl;
		if (INST.I == 0x0000006f) {	// Saltar sobre la misma dirección es la forma de terminar el programa
			jump = jump;			// este breakpoint para que la simulación no continue
			printf("Tiempo: %.0lf\t Numero de instrucciones: %d\n", tiempo, *numInst);
			printf("Valor de x10 = %d\n",(int)regs[10]);
			if ((int)regs[10] == 0) printf("La ejecucion es correcta\n");
			else printf("Parece que ha habido algun error\n");
			// inpeccionar x10 (a10) y comprobar que vale 0 para ejecucion correcta
			sc_stop();
			
		}

		jalOffset.bit(20) = I.bit(31);
		jalOffset(10, 1) = I(30, 21);
		jalOffset.bit(11) = I.bit(20);
		jalOffset(19, 12) = I(19, 12);
		jalOffset.bit(0) =	0;
		PCout.write(PCin.read() + jalOffset); // jalOffset will be sign-extended

		jump = true; 		
		C_opA = (sc_int<32>)PCin.read();
		C_opB = 4;  
		C_rd = (I(11, 7));		preWrite = true;
		preAlu = ADD;
		break;

	case 25: // JALR

		inm12 = I(31, 20);
		PCout.write(rs1 + inm12); // inm12 will be sign-extended

		jump = true; 		// hace falta algo más?

		C_opA = ((sc_int<32>)PCin.read());
		C_opB = (4);  //OJO
		C_rd = (I(11, 7));		preWrite = true;
		preAlu = ADD;
		uRs1 = true;	
		break;

	case 24: // Branches
		inmBR.bit(12) = I.bit(31);		inmBR.bit(11) = I.bit(7);
		inmBR(10, 5) = I(30, 25);		inmBR(4, 1) = I(11, 8);		inmBR.bit(0) = 0;
		preWrite = false;
		PCbranch = PCin.read();

		PCbranch += inmBR; 
		PCout.write(PCbranch);
								
		switch(I(14, 12)){
		case 0: 
			if (rs1 == rs2){ 
				jump = true;					strcpy(INST.desc, "beqY");} 	// BEQ
			else								strcpy(INST.desc, "beqN");
			break;
		case 1: 
			if (rs1 != rs2){ jump = true;		strcpy(INST.desc, "bneY");} 	// BNE
			else								strcpy(INST.desc, "bneN");
			break;
		case 4: 
			if (rs1 < rs2){ jump = true;		strcpy(INST.desc, "bltY");} 	// BLT
			else								strcpy(INST.desc, "bltN");
			break;
		case 5: 
			if (rs1 >= rs2){ jump = true;		strcpy(INST.desc, "bgeY");} 	// BGE
			else								strcpy(INST.desc, "bgeN");
			break;	
		case 6: 
			if ((sc_uint<32>)rs1 < (sc_uint<32>)rs2) { jump = true;		strcpy(INST.desc, "bltuY");} 	// BLTU
			else														strcpy(INST.desc, "bltuN");
			break;
		case 7: 
			if ((sc_uint<32>)rs1 >= (sc_uint<32>)rs2) { jump = true;	strcpy(INST.desc, "bgeuY");} 	// BGEU
			else														strcpy(INST.desc, "bgeuN");
			break;
		default: 
			cerr << "Unknown funct3 field in branch instruction: " << I(14, 12) << endl; 
		};
		uRs1 = true;	uRs2 = true;
		break;
	case 3: // Fence
		flagFence = true;
		preWrite = false;

		break;
	case 28: // ECALL, EBREAK, CSRR 

		switch (I(14, 12)) {

		case 1: // CSRRW
			rd = I(11, 7);
			csr = I(31, 27);

			regs[rd] = regs[csr];
			regs[csr] = rs1;

			break;
		case 2: // CSRRS
			rd = I(11, 7);
			csr = I(31, 27);

			regs[rd] = regs[csr];
			regs[csr] = (rs1 | regs[csr]);
			break;
		case 3: // CSRRC
			rd = I(11, 7);
			csr = I(31, 27);

			regs[rd] = regs[csr];
			regs[csr] = (rs1 & (~regs[csr]));
			break;
		case 5: // CSRRWi
			INST.rs1 = I(19, 15);
			INST.rs2 = I(11, 7); // rd, used for hazard detection
			rs1 = INST.rs1;

			rd = I(11, 7);
			csr = I(31, 27);

			regs[rd] = regs[csr];
			regs[csr] = rs1;
			break;
		case 6: // CSRRSi
			INST.rs1 = I(19, 15);
			INST.rs2 = I(11, 7); // rd, used for hazard detection
			rs1 = INST.rs1;

			rd = I(11, 7);
			csr = I(31, 27);

			regs[rd] = regs[csr];
			regs[csr] = (rs1 | regs[csr]);
			break;
		case 7: // CSRRCi
			INST.rs1 = I(19, 15);
			INST.rs2 = I(11, 7); // rd, used for hazard detection
			rs1 = INST.rs1;

			rd = I(11, 7);
			csr = I(31, 27);

			regs[rd] = regs[csr];
			regs[csr] = (rs1 & (~regs[csr]));
			break;

		default:
			rd = csr = 0;
			break;
		}

		preAlu = 0; preMem = 15;
		INST.rs1 = INST.rs2 = C_rd = 0x1f; C_wReg = false;
		INST.opA = INST.opB = INST.val2 = INST.aluOut = INST.dataOut = 0x0000dead;
		strcpy(INST.desc, "sys");
		preWrite = false;

		break;

	case 20: // EXTENSION RV32F - Floating point to integer - Floats only

		rd = I(11, 7);
		rs1 = I(19,15);
		rs2 = I(24,20);

		C_rd = rd;
		C_opA = regs[rs1];
		C_opB = regs[rs2];
		C_wReg = true;

		preWrite = true;
		uRs1 = true;
		INST.aluOp = 0;		INST.aluOut = 0x0000dead;

		strcpy(INST.desc, "pf_float");
		INST.target = 0x01;

		break;


	case 9: // FSW
		INST.target = 0x01;
		strcpy(INST.desc,"FSW");
		inm12(11, 5) = I(31, 25);	inm12(4, 0) = I(11, 7);
		C_opA = (rs1);
		C_opB = (inm12);
		C_rd = (I(11, 7));		preWrite = false;

		preAlu = ADD;
		preMem = I(14, 12) | 8; // write a word
		uRs1 = true;	uRs2 = true;

		break;

	case 1: // FLW
		INST.target = 0x01;
		strcpy(INST.desc, "FLW");
		preMem = I(14, 12); // read a word
		inm12 = I(31, 20);
		C_opA = rs1;
		C_opB = inm12;
		C_rd = I(11, 7);	preWrite = false;
		uRs1 = true;

		break;

	default:

		cerr << "Error, opCode " << opCode << " not supported" << endl;
		cerr << "    ERROR AT: " << INST << endl; 
	};

	// Hazard Detection with MUL module
	rs1Out.write(INST.rs1);
	rs2Out.write(INST.rs2);


	instruction iDX, iXM, iMU, iMW;
	iDX = fbEx.read();
	iMU = fbMul.read();
	iXM = fbMem.read();
	iMW = fbWB.read();

	// Hazard detection 
	idx_rs1 = (iDX.wReg && (iDX.rd == INST.rs1));
	ixm_rs1 = (iXM.wReg && (iXM.rd == INST.rs1));
	imw_rs1 = (iMW.wReg && (iMW.rd == INST.rs1));
	imu_rs1 = (iMU.wReg && (iMU.rd == INST.rs1));

	idx_rs2 = (iDX.wReg && (iDX.rd == INST.rs2));
	ixm_rs2 = (iXM.wReg && (iXM.rd == INST.rs2));
	imw_rs2 = (iMW.wReg && (iMW.rd == INST.rs2));
	imu_rs2 = (iMU.wReg && (iMU.rd == INST.rs2));

	// EmptyPipeline for Fence
	bool emptyPipeline = (readyFenceAluIn.read() && readyFenceMemIn.read() && readyFenceMulIn.read());

	if (!INST.rs1)
		hRs1 = false;
	else
		hRs1 = idx_rs1 || ixm_rs1 || imw_rs1 || imu_rs1 || hzrdRs1In.read() || hzrdPF_floatRs1In.read();

	if (!INST.rs2)
		hRs2 = false;
	else
		hRs2 = idx_rs2 || ixm_rs2 || imw_rs2 || imu_rs2 || hzrdRs2In.read() || hzrdPF_floatRs2In.read();

	if ((uRs1 && hRs1) || (uRs2 && hRs2) || (flagFence && !emptyPipeline)) { // hazard
		hazard.write(true);
		bubble.write(false);
		C_aluOp = (0);
		C_memOp = (15);
		C_wReg = (false); 		// the most important!
		INST.desc[0] = 'X';		INST.desc[1] = 0;
		PCout.write(PCin.read());	
	}
	else {
		if (jump) {
			hazard.write(false);
			bubble.write(true);
			C_aluOp = (0);
			C_memOp = (15);
			C_wReg = (preWrite); 
		}
		else {		// normal operation
			hazard.write(false);
			bubble.write(false);
			C_aluOp = (preAlu);
			C_memOp = (preMem);
			C_wReg = (preWrite);
		}
	}

}
