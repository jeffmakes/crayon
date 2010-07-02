#include <stdint.h>
#include <signal.h>
#include "device.h"
#include "wavegen.h"
uint16_t wavetable[WAVETABLE_SZ] = {10,20,30,40,50,60,70,80,90,10,110,120,130,140,150,160};

void wavegen_init()
{
  /* DAC bus is on P7 and P8, which together may be addressed as PA */
  PAOUT = 0;
  PADIR = 0xffff;		/* all outputs */

  TACTL = TASSEL_SMCLK		/* Source Timer A from SMCLK (= MCLK = 16MHz) */
    | MC_UPTO_CCR0;		/* Count to CCR0 and reset */
  TACCR0 = 4;			/* Overflow at 16/4=4MHz. Used to trigger DMA */
  TACCR1 = 2;
  TACCTL1 = OUTMOD_SET_RESET;	/* Create pulses at same frequency as overflow, but out of phase. These are used as the DACLAT signal to latch data into the external DAC.*/
  P1SEL |= (1<<6);		/* Enable TA1 output on P1.6 (DACLAT*/
  P1OUT |= (1<<6);		/* Enable output driver on P1.6 (DACLAT) */
     
  /* TBCTL = TBSSEL_SMCLK		/\* Source Timer B from SMCLK (= MCLK = 16MHz) *\/ */
  /*   | ID_DIV8			/\* divide by 8 = 2MHz *\/ */
  /*   | MC_UPTO_CCR0; */
  /* TBCCTL0 |= CCIE;		/\* Enable interrupt *\/ */
  /* TBCCR0 = 50000;		/\* 50ms Video frame end interrupt *\/ */

  DMACTL0 =  DMA0TSEL_TACCR0;	/* Trigger DMA (channel 0) transfer on TACCR0  */
  DMA0CTL = DMADT_SINGLEREPEAT 	/* Repeat-single-channel mode */
    | DMADSTINCR_0 		/* Don't increment destination address */
    | DMASRCINCR_3		/* Increment source address */
    /* DMADSTBYTE = 0 - destination is word */
    /* DMASRCBYTE = 0 - sourceis word */
    | DMAEN;			/* Enable DMA channel 0 */
  //| DMAIE;			/* Enable interrupt */
  DMA0SA = (uint16_t)&wavetable[0];     /* Source is the wavetable array*/
  DMA0DA = (uint16_t)&PAOUT;    /* Destination is DAC data bus*/
  DMA0SZ = (WAVETABLE_SZ-1);   /* Size of wave table */
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

