#include "device.h"
#include "stepper.h"
#include <signal.h>

#define EN (1<<0)
#define CONTROL (1<<1)
#define HF (1<<2)
#define DIR (1<<3)
#define CLOCK (1<<4)
#define nRESET (1<<5)
#define X_MOTOR 1
#define Y_MOTOR 0
/* Motor 0 is the right-hand driver, black connector. */
/* Motor 1 is the left-hand driver, red connector. */

void stepper_init()
{
  P1DIR = 0xff;
  P2DIR = 0xff;
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
}

void stepper_setxvelocity(uint16_t speed, uint8_t direction)
{
  if (speed == 0)
    stepper_disable(X_MOTOR);
  else
    {
      stepper_setdir(X_MOTOR, direction);
      stepper_enable(X_MOTOR);
      TACCR0 = 65535 - speed;
    }
}

interrupt (TIMERA1_VECTOR) stepper_stepinterrupt(void)
{
  TACTL &= ~TAIFG;
}

void stepper_setdir(uint8_t motor, uint8_t direction)
{
  if (motor == 0)
    {
      if (direction)
	P1OUT |= DIR;
      else
	P1OUT &= ~DIR;
    }
  if (motor == 1)
    {
      if (direction)
	P1OUT |= DIR;
      else
	P1OUT &= ~DIR;
    }
}

void stepper_step(uint8_t motor, uint8_t direction)
{
  volatile uint8_t count;
  if (motor == 0)
    {
      if (direction)
	{
	  P1OUT |= DIR;
	  P1OUT |= CLOCK;
	  for (count = 0; count < 5; count++);
	  P1OUT &= ~CLOCK;
	}
      else
	{
	  P1OUT &= ~DIR;
	  P1OUT |= CLOCK;
	  for (count = 0; count < 5; count++);
	  P1OUT &= ~CLOCK;
	}
    }
  else if (motor == 1)
    {
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
  if (motor == 0)
    {
      P1OUT &= ~EN;
      P1OUT &= ~nRESET;
    }
  if (motor == 1)
    {
      P2OUT &= ~EN;
      P2OUT &= ~nRESET;
    }
}




