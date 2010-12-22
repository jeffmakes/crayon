#include "device.h"
#include "stepper.h"
#include "print.h"
#include <signal.h>

#define EN (1<<0)
#define CONTROL (1<<1)
#define HF (1<<2)
#define DIR (1<<3)
#define CLOCK (1<<4)
#define nRESET (1<<5)

volatile uint16_t carriagepos;
uint8_t xstate;

/* Motor 0 is the right-hand driver, black connector. */
/* Motor 1 is the left-hand driver, red connector. */

void bodge_init();

void stepper_init()
{
  carriagepos = 1000;
  xstate = 0;

  P1DIR = EN | CONTROL | HF | DIR | CLOCK | nRESET;
  P2DIR = EN | CONTROL | HF | DIR | nRESET;
  P1OUT = HF;   			/* Fast decay, half step */
  P2OUT = HF;
  
  TACTL = 
    TASSEL_SMCLK 		/* Clock TA from SMCLK =16MHz */
    | ID_DIV8			/* /8 = 2MHz */
    | MC_UPTO_CCR0		
    | TAIE;

  TACCR0 = 40000;		/* 50Hz to start with... */
  TACCTL0 = OUTMOD_TOGGLE;	/* Toggle TA0 (P2.7) with TA overflow (clocks X stepper)*/
  P2SEL |= (1<<7);
  P2DIR |= (1<<7);

  P6DIR &= ~(1<<5);
  P6REN |= (1<<5);		/* Enable pull up resistor on the home switch */
  P6OUT |= (1<<5);		/* Pull _up_ */

  bodge_init();
}

void stepper_xhome()
{
  volatile uint16_t i = 0;

  stepper_setxvelocity(SPEED_SLOW, CARRIAGE_RIGHT);
  while ( !stepper_ishome() )
    i++;
 
  stepper_setxvelocity(0, 0);
  carriagepos = 0;
}

void stepper_moveXto(uint16_t newpos, uint16_t speed)
{
  if (newpos > carriagepos)
    stepper_setxvelocity(speed, CARRIAGE_LEFT);
  if (newpos < carriagepos)
    stepper_setxvelocity(speed, CARRIAGE_RIGHT);
  
  while (carriagepos != newpos);

  stepper_setxvelocity(0,0);
 }

void stepper_setxvelocity(uint16_t interval, uint8_t direction)
{
  if (interval == 0)
    {
      stepper_disable(X_MOTOR);
      xstate = STOPPED;
    }
  else
    {
      stepper_enable(X_MOTOR);
      if (direction)
	{
	  xstate = CARRIAGE_LEFT;
	  P2OUT |= DIR;
	}
      else
	{
	  xstate = CARRIAGE_RIGHT;
	  P2OUT &= ~DIR;
	}
      TACCR0 = interval;
    }
}

interrupt (TIMERA1_VECTOR) stepper_stepinterrupt(void)
{
  static uint8_t level = 0;
  level ^= 1;
  if (level) {
    /* 
     * interrupt fires on every transition, resulting in 2 print_process calls
     * per step.  This if lets only half pass
     */
    return;  
  }

  switch (xstate)
    {
    case CARRIAGE_LEFT:
      carriagepos++;
      break;
    case CARRIAGE_RIGHT:
      carriagepos--;
      break;
    }

  print_nextpixel();
  TACTL &= ~TAIFG;
}

void bodge_init()
{
  P1DIR |= (1<<7) | (1<<6);
  P3DIR |= (1<<7) | (1<<6);
}

#define enA(x) do { if (x) P1OUT |= (1<<7); else P1OUT &= ~(1<<7); } while (0)
#define enB(x) do { if (x) P3OUT |= (1<<7); else P3OUT &= ~(1<<7); } while (0)
#define phA(x) do { if (x) P1OUT |= (1<<6); else P1OUT &= ~(1<<6); } while (0)
#define phB(x) do { if (x) P3OUT |= (1<<6); else P3OUT &= ~(1<<6); } while (0)
#define X 1

void stepper_bodge_ystep(uint8_t direction, uint8_t steps)
{
  static uint8_t phase = 0;
  uint8_t i = 0 ;
  volatile uint32_t delay = 0;
  /* inverted signals EN and PH! */
  for (i = 0; i < steps; i++)
    {
      switch (phase)
	{
	case 0:
	  enA(0);
	  phA(1);
	  enB(1);
	  phB(X);
	  break;
	case 1:
	  enA(1);
	  phA(X);
	  enB(0);
	  phB(1);
	  break;
	case 2:
	  enA(0);
	  phA(0);
	  enB(1);
	  phB(X);
	  break;
	case 3:
	  enA(1);
	  phA(X);
	  enB(0);
	  phB(0);
	  break;
	}
      if (++phase == 4)
	phase = 0;
      for (delay = 0; delay<1000; delay++);
    }
}

void stepper_Ystep( uint8_t direction, uint8_t steps)
{
  volatile uint8_t count, i;
  if (direction)
      P1OUT |= DIR;
  else
      P1OUT &= ~DIR;

  for (count = 0; count < steps; count++ )
    {
      P1OUT |= CLOCK;
      for (i = 0; i < 5; i++);
      P1OUT &= ~CLOCK;

      for (i=0; i<255; i++);	/* Delay to allow stepper to move */
    }
}

void stepper_xstep( uint8_t direction)
{
  volatile uint8_t count;
  if (direction)
    {
      P2OUT |= DIR;
      P2OUT |= CLOCK;
      for (count = 0; count < 5; count++);
      P2OUT &= ~CLOCK;
    }
  else
    {
      P2OUT &= ~DIR;
      P2OUT |= CLOCK;
      for (count = 0; count < 5; count++);
      P2OUT &= ~CLOCK;
    }
}

void stepper_enable(uint8_t motor)
{
  if (motor == 0)
    {
      P1OUT |= EN;
      P1OUT |= nRESET;
    }
  if (motor == 1)
    {
      P2OUT |= EN;
      P2OUT |= nRESET;
    }
}

void stepper_disable(uint8_t motor)
{
  if (motor == 0)		/* Y */
    {
      P1OUT &= ~EN;
      P1OUT &= ~nRESET;
    }
  if (motor == 1)		/* X */
    {
      P2OUT &= ~EN;
      P2OUT &= ~nRESET;
    }
}


uint16_t stepper_getXpos() 
{
  return carriagepos;
}

