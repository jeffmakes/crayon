#include <msp430x20x3.h>
#include <signal.h>
#include <stdint.h>

/* Initialises everything. */
void init(void);

int i = 0;

int main( void )
{

  static uint16_t count = 0;
  init();
  
  while (1)
    {
    }
}

void init(void)
{
  /* GPIO: All inputs */
  P1DIR = 0;
  
  /* Disable the watchdog timer */
  //  WDTCTL = WDTHOLD | WDTPW;


  /* Use a 16 MHz clock (DCO) */
  DCOCTL = CALDCO_16MHZ;
  BCSCTL1 &= ~0x0f;
  BCSCTL1 |= 
    /*YT2O=0: YT2 is on*/
    /*YTS=0: LFYT1 mode select. 0 -Low frequency mode*/
    DIVA_0 /* ACLK Divider 0: /1 */
    |CALBC1_16MHZ; /* BCSCTL1 Calibration Data for 16MHz */
  
  BCSCTL2 = SELM_DCOCLK	/* MCLK from DCO */
    /* DIVMx = 0 : No MCLK divider */
    /* SELS = 0: SMCLK from DCO */
    | DIVS_DIV4/* : Divide SMCLK by 1 = 16MHz*/
    /* DCOR = 0 : DCO internal resistor */;

  stepper_init();
  draw_init();
  eint();
}

