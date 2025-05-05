#include <msp430x43x.h>
#include <signal.h>
#include <stdint.h>
#include "adc.h"

//array to store adc values
unsigned int adcbuffer[ADCBUFFSZ];
unsigned char adcwritepointer = 0;

void adc_init()
{
  unsigned int i = 0;
 
  // Enable ADC channels A7 and A0
  P6SEL = 0x81;   
  
  //Need >3.25us sample time (26 ticks) SHT0_3 gives 32
  //SHT0_5 gives 96 ticks = 12us
  //Set up 2.5V reference and turn on ADC. Enable multiple sample mode
  ADC12CTL0 = ADC12ON | SHT0_DIV512 | REFON | REF2_5V |MSC;  
  
  //Use sampling timer clocked from MCLK.
  //Use repeated single channel mode sampling into address 0
  ADC12CTL1 = CSTARTADD_0 | SHP | ADC12SSEL_MCLK | ADC12DIV_7 | CONSEQ_REPEAT_SINGLE;     
  
  //Use AVss and Vref as references for ADC channel 0 
  ADC12MCTL0 = SREF_1 | INCH_0;                  

  //Enable interrupt for ADC0
  ADC12IE = 0x0001;

    //wait for ADC reference startup
  for ( i=0; i<0x3600; i++);
  // Start ADC conversions
  ADC12CTL0 = ADC12CTL0 | ENC | ADC12SC;                     
}

void adc_writebuff(unsigned int value)
{
  //write to buffer and increment write pointer
  adcbuffer[adcwritepointer++] = value;
  
  //wrap round when end of buffer is reached
  if (adcwritepointer == ADCBUFFSZ)
    adcwritepointer = 0;
}

uint16_t adc_getaverage()
{
  uint8_t i = 0;
  uint32_t sum;

  sum = 0;
  for (i=0; i<ADCBUFFSZ; i++)
    sum += adcbuffer[i];
  return (uint16_t)(sum / ADCBUFFSZ);
}

//an interrupt is generated on each new
//ADC result. this funtion writes the new value
//to the ADC result buffer
interrupt (ADC12_VECTOR) adc_newresult(void)
{
  adc_writebuff(ADC12MEM0);
  
  
  ADC12IFG = 0;
}
