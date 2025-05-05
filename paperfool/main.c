/*
 * Copyright (c) 2025 Jeff Gough
 *
 * This software is released under the MIT License.
 * See the LICENSE file for full license text.
 */

#include <msp430x20x3.h>
#include <signal.h>
#include <stdint.h>

/* Initialises everything. */
void init(void);

volatile uint32_t thetime=0;
typedef enum {IDLE, PAGE_LOAD_DELAY, PRINTING} state_t;

int main( void )
{
  static state_t state = IDLE;
  static uint32_t edgetime, delay;
  uint8_t count = 0;
  volatile uint16_t startup_time = 0;

  for (startup_time=0; startup_time<60000; startup_time++);
  
  init();

  state = IDLE;
  thetime = 0;
  edgetime = 0;

  for (count = 0; count<10; count++)
    {
      delay = thetime;
      while (thetime < delay + 10)
	WDTCTL = WDTPW | WDTCNTCL;

      P1OUT ^= 1;
    }

  while (1)
    {
      WDTCTL = WDTPW | WDTCNTCL;
      switch(state)
	{
	case IDLE:
	  //release page sensor
	  P1DIR = (1<<0);
	  P1OUT = 0;
	  if (!(P1IN & 2))	/* Wait for !EN signal to go low */
	    {
	      edgetime = thetime;
	      state = PAGE_LOAD_DELAY;
	    }
	  break;
	case PAGE_LOAD_DELAY:
	  if (thetime > (edgetime+200))
	    {
	      edgetime = thetime;
	      state = PRINTING;
	    }
	  break;
	case PRINTING:
	  //assert page sensor
	  P1DIR = (1<<0) | (1<<2); /* Un-tristate P1.2, pulling down the opto sensor signal */
	  P1OUT = (1<<0) | (1<<2);
	  if (!(P1IN & 2))
	    edgetime = thetime;
	  if (thetime > (edgetime+200))
	    state = IDLE;
	  break;
	}
    }
}

interrupt (TIMERA1_VECTOR)hundredhertz(void)
{
  // This interrupt fires at 100Hz
  thetime++;

  TACTL &= ~TAIFG;
}

void init(void)
{
  P1DIR = (1<<0);
  
  /* Disable the watchdog timer */
  //  WDTCTL = WDTHOLD | WDTPW;

  /* Use a 1 MHz clock (DCO) */
  DCOCTL = CALDCO_1MHZ;
  BCSCTL1 &= ~0x0f;
  BCSCTL1 |= 
    /*YT2O=0: YT2 is on*/
    /*YTS=0: LFYT1 mode select. 0 -Low frequency mode*/
    DIVA_0 /* ACLK Divider 0: /1 */
    |CALBC1_16MHZ; /* BCSCTL1 Calibration Data for 16MHz */
  
  BCSCTL2 = SELM_DCOCLK	/* MCLK from DCO */
    /* DIVMx = 0 : No MCLK divider */
    /* SELS = 0: SMCLK from DCO */
    | DIVS_DIV8/* : Divide SMCLK by 8 = 2MHz*/
    /* DCOR = 0 : DCO internal resistor */;

  TACTL = 
    TASSEL_SMCLK 		/* Clock from SMCLK */
    | ID_DIV8			/*  /8 = 250kHz*/
    | MC_UPTO_CCR0		/* Reset at CCR0 value */
    | TAIE;			/* Enable overflow interrupt */

  TACCR0 = 2500;		/* 10Hz reset of timer A */
  eint();
}

