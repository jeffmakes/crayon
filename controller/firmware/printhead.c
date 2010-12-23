#include "printhead.h"
#include "device.h"
#include "wavegen.h"

#define NCHG 0
#define SIBK 1
#define SICL 1
#define CK 3
#define LAT 0
#define CH 2


extern uint8_t bk_data[K_NOZZLES];
extern uint8_t cl_data[C_NOZZLES + M_NOZZLES + Y_NOZZLES];
//uint8_t greytable[16] = {1,0,0,0,   1,1,0,0,   1,0,0,0,   1,1,1,0};
const uint8_t greytable[16] = {0,0,1,0,  0,0,0,0,  0,0,0,0,  0,0,0,0};
//uint8_t greytable[16] = {1,0,0,0,  0,0,1,0,  1,1,0,0,  0,0,0,1};

#define nchg(x) do { if (x) P3OUT |= (1<<NCHG); else P3OUT &= ~(1<<NCHG); } while (0)
#define sibk(x) do { if (x) P3OUT |= (1<<SIBK); else P3OUT &= ~(1<<SIBK); } while (0)
#define ch(x) do { if (x) P3OUT |= (1<<CH); else P3OUT &= ~(1<<CH); } while (0)
#define ck(x) do { if (x) P3OUT |= (1<<CK); else P3OUT &= ~(1<<CK); } while (0)
#define ck_edge() do { P3OUT ^= (1<<CK); } while (0)
#define lat(x) do { if (x) P5OUT |= (1<<LAT); else P5OUT &= ~(1<<LAT); } while (0)
#define sicl(x) do { if (x) P5OUT |= (1<<SICL); else P5OUT &= ~(1<<SICL); } while (0)

void printhead_init()
{
  uint8_t i = 0;

  P3DIR |= 1<<NCHG | 1<<SIBK | 1<<CH | 1<<CK;
  P5DIR |= 1<<LAT | 1<<SICL;

  nchg(0);
  sibk(0);
  sicl(0);
  ck(0);
  lat(0);
  ch(0);
  
  for (i=0;i<K_NOZZLES;i++)
    bk_data[i] = 0;
  for (i=0;i<C_NOZZLES + M_NOZZLES + Y_NOZZLES;i++)
    cl_data[i] = 0;
  
  P1DIR |= (1<<7);
}

void printhead_period()
{
  uint8_t nozzle,i;

  /* The printhead is designed to accept data for one print period (one horizontal pixel
   whilst printing the previous one. For now, we'll use a simple sequential load, latch, 
   print. Less time efficient, easier to understand */

  /* Each nozzle receives a two-bit greyscale value, stored in the bk_ and cl_data arrays */

  P1OUT |= (1<<7);

  for (nozzle = 0; nozzle < K_NOZZLES; nozzle++) /* high data bits, clocked on both edges of ck */
    {
      //sibk( (bk_data[nozzle] & 2) );
      //sicl( (cl_data[nozzle] & 2) );
      sibk(0);
      sicl(0);
      
      ck_edge();
    }
  for (nozzle = 0; nozzle < K_NOZZLES; nozzle++) /* low data bits, clocked on both edges of ck */
    {
      sibk( (bk_data[nozzle] & 1) );
      //sicl( (cl_data[nozzle] & 1) );
      sicl(0);
      ck_edge();
    }
  for (i = 0; i<16; i++)	/* grey mapping table. see logbook for the details of how this is calculated */
    {
      sibk( (greytable[i] & 1) );
      sicl( (greytable[i] & 1) );
      ck_edge();
    }
  
  lat(1);			/* Latch in the data */
  lat(0);
  P1OUT &= ~(1<<7);

  nchg(1);					  /* release nozzle charge maintain */  
  wavegen_drop();		/* droplet */
  ch(1);			/* increment subpixel counter */
  ch(0);
  wavegen_drop();		/* droplet */
  ch(1);			/* increment subpixel counter */
  ch(0);
  wavegen_drop();		/* droplet */
  ch(1);			/* increment subpixel counter */
  ch(0);
  wavegen_drop();		/* droplet */
  nchg(0);			/* re-enable nozzle charge maintain */
}

