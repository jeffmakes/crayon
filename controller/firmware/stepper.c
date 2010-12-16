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
#define CARRIAGE_LEFT 1
#define CARRIAGE_RIGHT 0
#define STOPPED 3;

uint16_t carriagepos;
uint8_t xdir;

/* Motor 0 is the right-hand driver, black connector. */
/* Motor 1 is the left-hand driver, red connector. */

void stepper_init()
{
  carriagepos = 0;

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
}

void stepper_xhome()
{
  stepper_setxvelocity(4000, CARRIAGE_RIGHT);
  while ( !stepper_ishome() );
  stepper_setxvelocity(0, 0);
}

void stepper_setxvelocity(uint16_t interval, uint8_t direction)
{
  if (interval == 0)
    stepper_disable(X_MOTOR);
  else
    {
      stepper_setdir(X_MOTOR, direction);
      stepper_enable(X_MOTOR);
      TACCR0 = interval;
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
	P2OUT |= DIR;
      else
	P2OUT &= ~DIR;
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




