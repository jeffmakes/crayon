#ifndef __PRINTHEAD_H
#define __PRINTHEAD_H
#include <stdint.h>

#define K_NOZZLES 90
#define C_NOZZLES 29
#define M_NOZZLES 29
#define Y_NOZZLES 29

uint8_t bk_data[K_NOZZLES];
uint8_t cl_data[C_NOZZLES + M_NOZZLES + Y_NOZZLES];

void printhead_init();
void printhead_period();

#endif
