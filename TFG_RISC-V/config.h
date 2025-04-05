#ifndef CONFIG_H
#define CONFIG_H


// GENERAL
#define BENCHMARKS 0

#define DEBUG 0

#define TRAZAS 0


// MUL MODULE
#define latencyMUL		2
#define latencyMULH		2
#define latencyMULHU	1
#define latencyMULHSU	1
#define latencyDIV		12

// rev todos 2 ej xogar con eso

constexpr int pipelineSizeMul = std::max({latencyMUL,latencyMULH,latencyMULHU,latencyMULHSU, latencyDIV});

/* Si 1 -> Solo puede estar ejecutandose una division/multiplicacion 
* y no puede entrar ninguna otra op
* Si 0 -> Solo puede haber una div, pero puede entrar otra mul
*/
#define SOLO_1OP 1

#endif 

// AUX PRINTS
/*
 cout << hex << INST.address << "  " << INST.rd << "   " << INST.desc << "  " << INST.opA << " " << INST.opB << "  " << tiempo << endl;

 //printf("xxx.cpp: M %2d <- %08x   @ %.0lf   -  %08x   %s   %08x op %08x \n", target, regs[target].to_int(), sc_time_stamp().to_double() / 1000.0, backInst.address.to_int(), backInst.desc, backInst.opA.to_int(), backInst.opB.to_int());

*/