//routines for handling PWM generation

#include <msp430x43x.h>
#include "pwm.h"

void pwm_set(unsigned int pwmvalue)
{
  //set compare register to new pwm value
  TA0CCR1 = pwmvalue;

  //sort out timer if it's higher than the new pwm value
  if (TAR > pwmvalue)
    TAR = pwmvalue-10;
}

void pwm_init()
{
  //set up timer A
  //source from SMCLK. Count up to CCR0 value
  TACTL = TASSEL_SMCLK | MC_UPTO_CCR0 | ID_0;
  
  //set up output modules 0 and 1 to generate PWM
  TACCTL0 = OUTMOD_TOGGLE;
  TACCTL1 = OUTMOD_RESET_SET;

  //set up CCR0 for PWM period
  TACCR0 = PWM_PERIOD; 
  //set CCR1 to initial PWM value
  TACCR1 = PWM_INIT;  
  
}

