#include "device.h"
#include "stepper.h"
#include "wavegen.h"
#include "printhead.h"
#include "print.h"
#include "buttons.h"
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
  volatile uint32_t j,i,k, drops, steps;
  uint8_t nozzle =0;
  uint8_t startprint = 0;
  uint8_t leds,dir;

  init();
  P5DIR = 0xff;
  P5OUT &= 0x0f;

  stepper_setxvelocity(0, 0);

  while (startprint == 0)
    {
      i = 100000;
      while ( (startprint == 0) && (i > 0))
	{
	  startprint = buttons_getstate();
	  i--;
	}

      P5OUT ^= (1<<7);
    }

  stepper_xhome();
  print_image();
  stepper_xhome();

  while(1);

  while (1)
    {
      //      print_cleancycle();
      //      for (i=0; i<5; i++)
      //	{
	  //	  print_nozzle_test();
      //	  print_line();
	  //	}
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
    | DIVS_DIV1 /* : Divide SMCLK by 1
*/
    /* DCOR = 0 : DCO internal resistor */;

  //  SVSCTL = VLD_3 | PORON;		/* SVS at 2.2V */
  
  buttons_init();
  wavegen_init();
  printhead_init();
  stepper_init();
  stepper_disable(X_MOTOR);
  stepper_disable(Y_MOTOR);

  eint();
}

