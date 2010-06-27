#include <signal.h>
#include <msp430x43x.h>

#include "lcd.h"
#include "adc.h"
#include "pwm.h"

void init(void);

//gdb bug fix - falls over if there is an empty .data section
static unsigned int dummy=1;


unsigned char p_pid = 0;

interrupt (PORT2_VECTOR) port2isr(void)
{
  if (P2IFG & 1<<1) //button 1 pressed
    //    bees++;
  if (P2IFG & 1<<2) //button 2 pressed
    //be/es--;
  if (P2IFG & 1<<3); //button 3 pressed
    
  P2IFG=0;//interrupts handled, clear flags
  // lcdvalue(bees);
}


//Fires at 400Hz (CCR0 timer reset)
interrupt (TIMERA0_VECTOR) pwm_periodinterrupt()
{
  #define SETPOINT 700
  static uint16_t counter = 0;
  uint16_t temperature = 0; // temperature = degrees C *10. Eg 300 = 30 degrees C

  uint8_t kp = 60;
  uint8_t ki = 5;
  uint8_t kd = 80;
  static int32_t perror = 0;
  static int32_t ierror = 0;
  static int32_t derror = 0;  
  static int32_t prev_perror = 0;
  static uint8_t itimeout = 90;
  static uint8_t dtimeout;
  static int32_t pid = 0;
  static int32_t pwmout = 0;
 
  if (counter++ == 40)
    {
      counter = 0;
      // Every 100ms...      
      temperature = (uint16_t)((uint32_t)(adc_getaverage()) * 2500 / 4095);
      
      //display temperature
      lcd_dispvalue((uint16_t)temperature);

      perror = (int32_t)SETPOINT - temperature;
      derror = perror - prev_perror;
      if (itimeout++ == 100)
	{
	  if ((pwmout != PWM_MAX) && (pwmout != PWM_MIN))
	    ierror += perror;
	  itimeout = 0;
	}
      if (dtimeout++ == 50)
	{
	  prev_perror = perror;
	  dtimeout = 0;
	}

      pid = (kp * perror) + (ki * ierror) + (kd * derror); 
      pwmout = pid;
      if (pwmout > PWM_MAX)
	pwmout = PWM_MAX;
      if (pwmout < PWM_MIN)
	pwmout = PWM_MIN;

      pwm_set((uint16_t)pwmout);
    }
  
  TACCTL0 &= ~CCIFG;
}


int main(void) 
{
  unsigned long vin=0;
   
  //PID variables
  signed long perror = 0;
  signed long prev_perror = 0;
  signed long ierror = 0;
  signed long derror = 0;
  volatile signed long kp = 30;
  signed long ki = 4;
  signed long kd = 20;
  signed long pterm = 0;
  signed long iterm = 0;
  signed long dterm = 0;
  signed long pid = 0;
  signed long new_pwmout = 0;
  unsigned int pid_div = 30;
  
  unsigned int itimer = 0;
  unsigned int itimeout = 1;
  unsigned char dtimer = 0;
  unsigned char dtimeout = 1;
  unsigned char lcdtimer = 0;
  unsigned char lcdtimeout = 50;

  unsigned char pwm_min = 0;
  unsigned int pwm_max = 270;

  unsigned char nowind = 0;
    
  unsigned char dead = 4;

  unsigned char bias=0;

  init();
  lcd_clear();

  while (1) 
    {

      /*
      //find median vin value
      vin = *filter(adcbuffer);
           
      //proportional error
      perror = (signed long)(SETPOINT - vin);

      //deadband
      if (abs(perror) < dead)
	perror=0;
      
      //integral error
      if ((itimer++) == itimeout)
      {
	//don't add to ierror when output is saturated to avoid windup
	if (nowind || ((pwmout != pwm_max) && (pwmout != pwm_min)))
	  ierror = ierror + perror;
	itimer = 0;
      }

      //derivative error
      derror = perror - prev_perror;
      
      //compute differential after some number of PID cycles
      if ((dtimer++) == dtimeout)
	{
	  prev_perror = perror;
	  dtimer = 0;
	}
      
      dint();
      //proportional term
      pterm = kp * perror;
      //integral term
      iterm = ki * ierror;
      //derivative term
      dterm = kd * derror;
      eint();

      //compute PID result
      //result is full signed integer range ~ -32000 to +32000
      //PWM values can be 0 to PWM_MAX (~400)
      pid = pterm + iterm + dterm;
      
      //new_pwmout = (signed int)pwmout + pid;
      new_pwmout = pid / (signed long)pid_div + bias;

      //and keep to PWM limits
      if (new_pwmout > (signed long)pwm_max)
	new_pwmout = (signed long)pwm_max;
      if (new_pwmout < (signed long)pwm_min)
	new_pwmout = (signed long)pwm_min;

      pwm_set((unsigned int)new_pwmout);
      */
    }

}


void init(void)
{
  //clock setup
  //with DCOPLUS=1, Fdcoclk = D * (N+1) * Faclk
  FLL_CTL0 = XCAP18PF | DCOPLUS; //set up crystal load caps and FLL division
  FLL_CTL1 = 0;  
  //we're driving aclk from 32kHz crystal. N=60, D=4 gives operating freq of Fdcoclk = 7995392
  SCFQCTL = 60;
  SCFI0 = FN_4 | FLLD_4;
  
  //disable watchdog timer
  WDTCTL = WDTPW| WDTHOLD;
  
  //port function select registers.
  P1SEL = 0x0e;
  P2SEL = 0x10;


  //port direction registers
  P1DIR  = 0x04;
  P2DIR  = 0x10;

  //interrupt on button press on port 2
  P2IE   = 0x0e;

  //disable interrupts
  dint();

  //initialise other modules
  lcd_init();
  adc_init();
  pwm_init();
  //  uart_init();
  
  eint(); //enable interrupts
}
