#include "device.h"
#include "video.h"
#include "draw.h"
#include <signal.h>
#include "/usr/msp430/include/msp430/svs.h"


/* Initialises everything. */
void init(void);

int i = 0;

int main( void )
{
  int16_t frame = 0 ;
  int8_t frameincrement = 4;
  uint8_t vidbackwards = 0;
  init();

  P1OUT = 0;

  clear_buffer(fb0);
  clear_buffer(fb1);
  draw_unpackframe(frame, drawfb);
  while (1)
    {
      /* wait for frame-change compare */
      while ( !(TBCCTL0 & CCIFG) );
      TBCCTL0 &= ~CCIFG;
      /* swap framebuffers */
      frameswitch=1;

      while(frameswitch);
      /* unpack next frame into loading buffer*/
      clear_buffer(loadfb);
      draw_unpackframe(frame, loadfb);

      if (vidbackwards)
	{
	  if ((frame-=frameincrement) <= 0)
	    {
	      frame = 0;
	      vidbackwards = 0;
	    }
	}
      else 
	{
	  if((frame+=frameincrement) >= (videolength-1))
	    {
	      frame = videolength - 1;
	      vidbackwards = 1;
	    }
	}
      
    }
}

void init(void)
{
  /* GPIO: All inputs */
  P1DIR = P2DIR = P3DIR = P4DIR = P5DIR = P6DIR = P7DIR = P8DIR = 0;
  P8SEL=0;
  
  /* Disable the watchdog timer */
  //  WDTCTL = WDTHOLD | WDTPW;


  /* Use a 16 MHz clock (DCO) */
  DCOCTL = CALDCO_16MHZ;
  BCSCTL1 &= ~0x0f;
  BCSCTL1 |= 
    /*XT2O=0: XT2 is on*/
    /*XTS=0: LFXT1 mode select. 0 -Low frequency mode*/
    DIVA_0 /* ACLK Divider 0: /1 */
    |CALBC1_16MHZ; /* BCSCTL1 Calibration Data for 16MHz */
  
  BCSCTL2 = SELM_DCOCLK	/* MCLK from DCO */
    /* DIVMx = 0 : No MCLK divider */
    /* SELS = 0: SMCLK from DCO */
    | DIVS_DIV2 /* : Divide SMCLK by 2 = 8MHz*/
    /* DCOR = 0 : DCO internal resistor */;

  /* row drivers are on P1 and the low 5 bits of P2 */
  P1OUT = 0;
  P2OUT = 0;
  P1DIR = 0xff;
  P2DIR = 0x1f;
  /* column drivers are on P7 and P8, which together may be addressed as PA */
  /* p-channel fets are level-translated with pull-up resistors, so keep PAOUT = 0 */
  /* and use PADIR register to assert or tristate. */

  PAOUT = 0;
  PADIR = 0;

  SVSCTL = VLD_3 | PORON;		/* SVS at 2.2V */


  draw_init();
  
  eint();
}

