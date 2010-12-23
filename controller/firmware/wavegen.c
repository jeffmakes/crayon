#include <stdint.h>
#include <signal.h>
#include "device.h"
#include "wavegen.h"
//Each step lasts 1/4MHz = 0.25us
const uint16_t wavetable[WAVETABLE_SZ] = 
  {500,475,450,425,400,375,350,325,300,275,250,225,200,175,150,125,100,75,50,25,0,0,0,0,0,0,0,0,125,250,375,500,625,750,875,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,975,950,925,900,875,850,825,800,775,750,725,700,675,650,625,600,575,550,525,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500};

void wavegen_init()
{
  volatile uint32_t i = 200000;
  P4OUT = 0;

  /* DAC bus is on P7 and P8, which together may be addressed as PA */
  PAOUT = 0;
  PADIR = 0xe3ff;		/* all outputs, except P8.2, which is DACLAT on
				   the PCB, and must be overridden by P4.4 and
				   P8.3 which is !RST on the PCB and must be overridden
				   by P4.5 and P8.4 which is PSAVE which must be overridden
				   by P4.6*/

  P4DIR |= (1<<5) | (1<<4); /* DACLAT, !RST */

  P4OUT &= ~(1<<5);	        /* Apply negative going pulse on !RST to reset DAC (necessary?) */
  i = 20000; while (i--);
  P4OUT |= (1<<5);
  P4SEL |= (1<<4);		/* Enable TB2 output on P4.4 (DACLAT) */

  /* The DMA controller is configured before the timer. Otherwise, the TBCCR2 CCIFG flag may already be set */     
  /* which is a problem because the DMA controller is edge triggered, will never be triggered, and will never reset CCIFG */
  DMACTL0 =  DMA0TSEL_TBCCR2;	/* Trigger DMA (channel 0) transfer on TBCCR2  */
  TBCCR2 = 1;			/* Transfer occurs on TB=1, then DACLAT happens at TB=2 */
  
  DMA0SA = (uint16_t)&wavetable[0];     /* Source is the wavetable array*/
  DMA0DA = (uint16_t)&PAOUT;    /* Destination is DAC data bus*/
  DMA0SZ = (WAVETABLE_SZ-1);   /* Size of wave table */
  DMA0CTL = DMADT_SINGLE 	/* Block transfer mode */
    | DMADSTINCR_0 		/* Don't increment destination address */
    | DMASRCINCR_3;		/* Increment source address */
       
    /* DMADSTBYTE = 0 - destination is word */
    /* DMASRCBYTE = 0 - sourceis word */
  //| DMAIE;			/* Enable interrupt */
  

  TBCTL = TBSSEL_SMCLK		/* Source Timer A from SMCLK (= MCLK = 16MHz) */
    | MC_UPTO_CCR0;		/* Count to CCR0 and reset */
  //| MC_CONT;
  TBCCR0 = 3;			/* Overflow at 16/4=4MHz. Used to trigger DMA */
  
  TBCCR4 = 2;
  TBCCTL4 = OUTMOD_SET_RESET;	/* Create pulses on TB4 at same frequency as overflow, but out of phase.
				   These are used as the DACLAT signal to latch data into the external DAC.*/
} 

void wavegen_cleanpulse()
{
  volatile uint32_t i = 0;
  P4SEL &= ~(1<<4);		/* Disable TB2 output on P4.4 (DACLAT) */

  PAOUT = 0;
  P4OUT |= (1<<4);		/* DACLAT high */
  P4OUT &= ~(1<<4);		/* DACLAT low */

  i = 400; while (i--);

  PAOUT = 0xffff;
  P4OUT |= (1<<4);		/* DACLAT high */
  P4OUT &= ~(1<<4);		/* DACLAT low */
  
  P4SEL |= (1<<4);		/* Enable TB2 output on P4.4 (DACLAT) */

  i = 400; while (i--);

}

void wavegen_drop()
{
  volatile uint8_t bee;
  TBCCTL2 &= ~CCIFG;		/* Clear TBCCR2 CCIFG */
  DMA0CTL &= ~DMAIFG;
  DMA0CTL |= DMAEN;		/* Enable DMA channel 0. */
  
  while (DMA0CTL & DMAEN)
    {
      TBCCTL2 &= ~CCIFG;		/* Clear TBCCR2 CCIFG */
      bee++;	/* Wait for end of transfer */
    }
}


 
/* interrupt (TIMERB0_VECTOR) tb_endofframe(void) */
/* { */
/*   /\* Every 50ms this isr fires and the video advances one frame *\/ */
  

/* } */

/* interrupt (DMA_VECTOR) dma_isr(void) */
/* { */
/* This interrupt fires when the dma transfer reaches the end of the table, so could use it to generate CH pulses. */
/*   DMA0CTL &= ~(DMAIFG); */
/* } */

