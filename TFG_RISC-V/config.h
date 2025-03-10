#ifndef CONFIG_H
#define CONFIG_H

#define latencyMUL 2
#define latencyMULHU 2
#define pipelineSize max(latencyMUL,latencyMULHU)

#define BENCHMARKS 0

#define DEBUG 0

#endif 

/*
* SPMV usando int y verify en lugar de verifyDouble
 Latencia 2 falla
 Correcto con 1,3,5,16,17
 Rev problema escritura?? --> Mod decod.cpp registros

 SPMV usando double y verifyDouble
 falla :(

*/