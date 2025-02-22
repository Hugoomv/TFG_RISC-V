#include "systemc.h"
#include <stdio.h>
#include <time.h>

#include "coreRiscV.h"

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

	if (instCoreRiscV.leeELF(elf)) {
		fclose(elf);
		cerr << "ERROR leyendo archivo " << vargs[1] << endl;
		exit(-1);
	}

	rst.write(true); sc_start(2, SC_NS);
	time(&begin);
	rst.write(false); sc_start(10, SC_SEC);
	time(&end);

	printf("Tiempo %ld\n", end - begin);


	return 0;
	   
}
#endif


