#include "device.h"
#include "stepper.h"

#define EN (1<<0)
#define CONTROL (1<<1)
#define HF (1<<2)
#define DIR (1<<3)
#define CLOCK (1<<4)
#define nRESET (1<<5)

/* Motor 0 is the right-hand driver, black connector. */
/* Motor 1 is the left-hand driver, red connector. */

void stepper_init()
{
  P1DIR = 0xff;
  P2DIR = 0xff;
  P1OUT = HF;   			/* Fast decay, full step */
  P2OUT = HF;
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




