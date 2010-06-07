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

interrupt (TIMERA1_VECTOR)tenhertz(void)
{
  // This interrupt fires at 10Hz
  P1OUT ^= 1;
}

void init(void)
{
  P1DIR = 1;
  
  /* Disable the watchdog timer */
  WDTCTL = WDTHOLD | WDTPW;

  /* Use a 1 MHz clock (DCO) */
  DCOCTL = CALDCO_1MHZ;
  BCSCTL1 &= ~0x0f;
  BCSCTL1 |= 
    /*YT2O=0: YT2 is on*/
    /*YTS=0: LFYT1 mode select. 0 -Low frequency mode*/
    DIVA_0 /* ACLK Divider 0: /1 */
    |CALBC1_1MHZ; /* BCSCTL1 Calibration Data for 16MHz */
  
  BCSCTL2 = SELM_DCOCLK	/* MCLK from DCO */
    /* DIVMx = 0 : No MCLK divider */
    /* SELS = 0: SMCLK from DCO */
    | DIVS_DIV4/* : Divide SMCLK by 4 = 250kHz*/
    /* DCOR = 0 : DCO internal resistor */;

  TACTL = 
    TASSEL_SMCLK 		/* Clock from SMCLK = 250kHz */
    | MC_UPTO_CCR0		/* Reset at CCR0 value */
    | TAIE;			/* Enable overflow interrupt */

  TACCR0 = 25000;		/* 10Hz reset of timer A */
  eint();
}

