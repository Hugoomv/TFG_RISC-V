#ifndef CONFIG_H
#define CONFIG_H

#define latencyMUL 2
#define latencyMULHU 2
#define pipelineSize max(latencyMUL,latencyMULHU)

#define BENCHMARKS 0

#define DEBUG 0

#define TRAZAS 0

#endif 

/*
  REV - Mod hazard detection - tanto en mul como decod
 REV - Org cond decod
 REV - Decod lat 2 mul ??  

 cout << hex << INST.address << "  " << INST.rd << "   " << INST.desc << "  " << INST.opA << " " << INST.opB << "  " << tiempo << endl;

 //printf("xxx.cpp: M %2d <- %08x   @ %.0lf   -  %08x   %s   %08x op %08x \n", target, regs[target].to_int(), sc_time_stamp().to_double() / 1000.0, backInst.address.to_int(), backInst.desc, backInst.opA.to_int(), backInst.opB.to_int());



*/