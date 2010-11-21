#include "printhead.h"
#include "device.h"

#define NCHG 0
#define SIBK 1
#define SICL 1
#define CK 3
#define LAT 0
#define CH 2

#define K_NOZZLES 90
#define C_NOZZLES 29
#define M_NOZZLES 29
#define Y_NOZZLES 29

uint8_t bk_data[K_NOZZLES];

#define nchg(x) do { if (x) P3OUT |= (1<<NCHG); else P3OUT &= ~(1<<NCHG); } while (0)

void printhead_init()
{
  nchg(0);
}

void printhead_period()
{
  
}
