#ifndef CPULOAD_H
#define CPULOAD_H

#include <stdbool.h>

#define MAXCPU 64
#define COLSEP "\t"

bool cpuload(int numcpu, bool output);

#endif // CPULOAD_H


