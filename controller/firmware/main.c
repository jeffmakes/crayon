#include "device.h"
#include "wavegen.h"
#include "printhead.h"
#include <signal.h>
#include "/usr/msp430/include/msp430/svs.h"

/* Initialises everything. */
void init(void);

int dummy = 0;

void cleancycle()
{
  uint8_t i,j,k;
  for (j=0;j<K_NOZZLES;j++)
    {
      for (i=0;i<K_NOZZLES;i++)	/* clear all nozzles */
	bk_data[i] = 0;
      //     bk_data[j] = 1;
      printhead_period();
      
      for (k=0; k<50; k++)
	wavegen_cleanpulse();
    }
}

int main( void )
{
  init();
  volatile uint32_t j,i,k, drops, steps;
  uint8_t nozzle =0;
  uint8_t cleantime = 0;
  uint8_t leds,dir;
  dir =1;
  
  P5DIR = 0xff;

  for (i=0;i<K_NOZZLES;i++)	/* Fire all nozzles */
    bk_data[i] = 1;

  
  steps = 0;

  while (1)
    {
      for (i=0;i<K_NOZZLES;i++)	/* Fire all nozzles */
	bk_data[i] = 1;
      for (i=0;i<50;i++);
      printhead_period();
      
      

      stepper_step(1,dir);
      if (++steps == 1300)
      	{
      	  dir ^= 1;
      	  steps=0;
	  /* if (++cleantime == 10) */
	  /*   { */
	  /*     cleancycle(); */
	  /*     cleantime = 0; */
	  /*   } */
	}

      if ((++leds)==17)
	leds = 0;
      //P5OUT = leds << 4;
    }
}

void init(void)
{
  /* GPIO: All inputs */
  P1DIR = P2DIR = P3DIR = P4DIR = P5DIR = P6DIR = P7DIR = P8DIR = 0;
  P8SEL=0;
  
  /* Disable the watchdog timer */
  WDTCTL = WDTHOLD | WDTPW;

  /* Use a 16 MHz clock (DCO) */
  DCOCTL = CALDCO_16MHZ;
  BCSCTL1 &= ~0x0f;
  BCSCTL1 |= 
    /*XT2O=0: XT2 is on*/
    /*XTS=0: LFXT1 mode select. 0 -Low frequency mode*/
    DIVA_0 /* ACLK Divider 0: /1 */
    | CALBC1_16MHZ; /* BCSCTL1 Calibration Data for 16MHz */
  
  BCSCTL2 = SELM_DCOCLK	/* MCLK from DCO */
    /* DIVMx = 0 : No MCLK divider */
    /* SELS = 0: SMCLK from DCO */
    | DIVS_DIV1 /* : Divide SMCLK by 1 = 16MHz*/
    /* DCOR = 0 : DCO internal resistor */;

  //  SVSCTL = VLD_3 | PORON;		/* SVS at 2.2V */
  wavegen_init();
  printhead_init();
  stepper_init();
  stepper_enable(1);
  //  eint();
}

