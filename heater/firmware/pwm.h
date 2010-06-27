#ifndef PWM_H
#define PWM_H

//set PWM period. At 8Mhz, 400 ~= 20kHz
#define PWM_PERIOD 400
//set PWM limits and initial value
#define PWM_MIN 0
#define PWM_MAX 380
#define PWM_INIT 20

//define english name for PWM output
#define pwmout TACCR1

//PWM initialisation code
void pwm_init();

//set PWM value
void pwm_set(unsigned int pwmvalue);

#endif
