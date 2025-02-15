#include "systemc.h"
#include <stdio.h>
#include <time.h>

#include "coreRiscV.h"



int sc_main(int nargs, char* vargs[]) {

	// Ignore deprecated alerts
	sc_core::sc_report_handler::set_actions("/IEEE_Std_1666/deprecated",
		sc_core::SC_DO_NOTHING);

	sc_clock clk("clk", 1); // ciclo de 1 ns
	sc_signal <bool> rst;
	FILE* elf; 
	time_t begin, end;

#if BENCHMARKS

	const char* benchmarks_array[] = {"Debug/Median.elf","Debug/Multiply.elf"};

	const int size_arr = sizeof(benchmarks_array) / sizeof(benchmarks_array[0]);


	for (int i = 0; i < size_arr;i++) {

		elf = fopen(benchmarks_array[i], "rb");
		if (!elf) {
			cerr << "ERROR. No puedo abrir el archivo " << benchmarks_array[i] << endl;
			exit(-1);
		}

		coreRiscV instCoreRiscV("core");
		

		instCoreRiscV.clk(clk);
		instCoreRiscV.rst(rst);

		if (instCoreRiscV.leeELF(elf)) {
			fclose(elf);
			cerr << "ERROR leyendo archivo " << benchmarks_array[i] << endl;
			exit(-1);
		}

		rst.write(true); sc_start(2, SC_NS);
		time(&begin);
		rst.write(false); sc_start(10, SC_SEC);
		time(&end);


		printf("Tiempo %ld\n", end - begin);

	
	}

	std::cout << "Finalizando el programa.\n";

#else

	
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

#endif

	return 0;
	   
}


