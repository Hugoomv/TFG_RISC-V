#include "systemc.h"
#include <stdio.h>
#include <time.h>

#include "coreRiscV.h"

#define trazas 

#ifdef trazas
#include <sysc/tracing/sc_vcd_trace.h>
#endif


#if BENCHMARKS
	
int sc_main(int nargs, char* vargs[]) {

	sc_clock clk("clk", 1); // ciclo de 1 ns
	sc_signal <bool> rst;
	FILE* elf;
	time_t begin, end;

	const char* nombresBenchmarks[] = {"Debug/Qsort.elf", "Debug/Multiply.elf" };		// ejemplos

	const int numELFs = sizeof(nombresBenchmarks) / sizeof(nombresBenchmarks[0]);

	coreRiscV* instCoreRiscV[numELFs] = {};

	for (int i = 0; i < numELFs; ++i) {
		instCoreRiscV[i] = new coreRiscV("core");
	}


	for (int i = 0; i < numELFs; ++i) {		// dar valor a numELFs

		elf = fopen(nombresBenchmarks[i], "rb");
		if (!elf) {
			cerr << "ERROR. No puedo abrir el archivo " << nombresBenchmarks[i] << endl;
			exit(-1);
		}

		cout << nombresBenchmarks[i] << endl << endl;


		//instCoreRiscV = new coreRiscV("core");		// lo instancio dinámicamente para poder eliminarlo después

		instCoreRiscV[i]->clk(clk);
		instCoreRiscV[i]->rst(rst);

		if (instCoreRiscV[i]->leeELF(elf)) {
			fclose(elf);
			cerr << "ERROR leyendo archivo " << nombresBenchmarks[i] << endl;
			exit(-1);
		}

		rst.write(true); sc_start(2, SC_NS);
		time(&begin);
		rst.write(false); sc_start(10, SC_SEC);
		time(&end);

		printf("Tiempo %ld\n", end - begin);

		//delete instCoreRiscV[i];					// lo borro para poder crearlo otra vez y así estar "seguro" de que cada benchmark empieza de cero

	}
	return 0;

}

#else


int sc_main(int nargs, char* vargs[]) {

	// Ignore deprecated alerts
	sc_core::sc_report_handler::set_actions("/IEEE_Std_1666/deprecated",
		sc_core::SC_DO_NOTHING);

	sc_clock clk("clk", 1); // ciclo de 1 ns
	sc_signal <bool> rst;
	FILE* elf; 
	time_t begin, end;


	
	if (nargs != 2) {
		cerr << "ERROR. Se debe especificar el archivo ELF" << endl;
		exit(-1);
	}

	elf = fopen(vargs[1], "rb");
	if (!elf) {
		cerr << "ERROR. No puedo abrir el archivo " << vargs[1] << endl;
		exit(-1);
	}

	coreRiscV instCoreRiscV("core");

	instCoreRiscV.clk(clk);
	instCoreRiscV.rst(rst);

#ifdef trazas
	sc_trace_file* Tf;
	Tf = sc_create_vcd_trace_file("traza");
	((vcd_trace_file*)Tf)->set_time_unit(0.5, SC_NS);
#endif

	if (instCoreRiscV.leeELF(elf)) {
		fclose(elf);
		cerr << "ERROR leyendo archivo " << vargs[1] << endl;
		exit(-1);
	}

#ifdef trazas
	sc_trace(Tf, clk, "clk");
	sc_trace(Tf, instCoreRiscV.hazard, "hazard");
	sc_trace(Tf, instCoreRiscV.bubble, "bubble");
	sc_trace(Tf, instCoreRiscV.rs1, "rs1");
	sc_trace(Tf, instCoreRiscV.rs2, "rs2");
	sc_trace(Tf, instCoreRiscV.hzrdRs1, "hzrdRs1");
	sc_trace(Tf, instCoreRiscV.hzrdRs2, "hzrdRs2");
	sc_trace(Tf, instCoreRiscV.iDX.read().address, "iDX.addr");
	sc_trace(Tf, instCoreRiscV.iDX.read().rs1, "iDX.rs1");
	sc_trace(Tf, instCoreRiscV.iDX.read().rs2, "iDX.rs2");
	sc_trace(Tf, instCoreRiscV.iDX.read().rd, "iDX.rd");
	sc_trace(Tf, instCoreRiscV.iDX.read().wReg, "iDX.wReg");

	sc_trace(Tf, instCoreRiscV.iMul.read().address, "iMul.addr");
	sc_trace(Tf, instCoreRiscV.iMul.read().rs1, "iMul.rs1");
	sc_trace(Tf, instCoreRiscV.iMul.read().rs2, "iMul.rs2");
	sc_trace(Tf, instCoreRiscV.iMul.read().rd, "iMul.rd");
	sc_trace(Tf, instCoreRiscV.iMul.read().wReg, "iMul.wReg");

	sc_trace(Tf, instCoreRiscV.iXM.read().wReg, "iXM.wReg");
	sc_trace(Tf, instCoreRiscV.iXM.read().rd, "iXM.rd");


	sc_trace(Tf, instCoreRiscV.iMW.read().wReg, "iMW.wReg");
	sc_trace(Tf, instCoreRiscV.iMW.read().rd, "iMW.rd");

	sc_trace(Tf, instCoreRiscV.instDecod->HZ1, "HZ1");
	sc_trace(Tf, instCoreRiscV.instDecod->HZ2, "HZ2");


//	sc_trace(Tf, instCoreRiscV.iXM, "iXM");
	//sc_trace(Tf, instCoreRiscV., "");

	
#endif


	rst.write(true); sc_start(2, SC_NS);
	time(&begin);
	rst.write(false); sc_start(10, SC_SEC);
	time(&end);

	printf("Tiempo %ld\n", end - begin);


	return 0;
	   
}
#endif


