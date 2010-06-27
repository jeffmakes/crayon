#include <signal.h>
#include <msp430x43x.h>

#include "lcd.h"
#include "adc.h"
#include "pwm.h"
#include "uart.h"
#include "filter.h"

#define SETPOINT 6000 / 3

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

int main(void) 
{
  unsigned long vin=0;
   
  //PID variables
  signed long perror = 0;
  signed long prev_perror = 0;
  signed long ierror = 0;
  signed long derror = 0;
  signed long kp = 30;
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
      //IE1=128;

      //find median vin value
      vin = *filter(adcbuffer);
      
      //display output voltage
      if ((lcdtimer++) == lcdtimeout)
	{
	  //scale to voltage for human reading
	  lcd_dispvalue(vin*3);
	  lcdtimer = 0;
	}
     
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

      //output debug info by serial if required
      if (p_pid)
	{	
	  uart_txwritebuff_char('q');
	  uart_txwritebuff_word((unsigned int)vin);
	  
	  uart_txwritebuff_char('r');      
	  uart_txwritebuff_word((unsigned int)pwmout);            
	}
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
  uart_init();
  
  eint(); //enable interrupts
}
