#include "device.h"
#include "wavegen.h"
#include <signal.h>
#include "/usr/msp430/include/msp430/svs.h"

/* Initialises everything. */
void init(void);

int dummy = 0;

int main( void )
{
  init();
  volatile uint32_t i = 1000;
  uint8_t leds;
  
  P5DIR = 0xff;
  while (1)
    {
      /* P4OUT &= ~(1<<5);	        /\* Apply negative going pulse on !RST to reset DAC (necessary?) *\/ */
      /* i = 200000;  */
      /* while (i--); */
      /* P4OUT |= (1<<5); */
      /* i = 200000; while (i--); */
      /* P4OUT |= (1<<6);		/\* Apply positive going pulse on PSAVE to get DAC to work. (Necessary, but not sure why.) *\/ */
      /* i = 200000; while (i--); */
      /* P4OUT &= ~(1<<6); */

      i = 200000; while (i--); 
      if ((++leds)==17)
	leds = 0;
      P5OUT = leds << 4;
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
  //  eint();
}

