/*
 * Copyright (c) 2025 Jeff Gough
 *
 * This software is released under the MIT License.
 * See the LICENSE file for full license text.
 */

#ifndef ADC_H
#define ADC_H
#include <stdint.h>

//size of ADC input buffer
#define ADCBUFFSZ 64

//adc buffer variables
unsigned int adcbuffer[ADCBUFFSZ];

//routines for managing the ADC buffer
void adc_writebuff(unsigned int value);
uint16_t adc_getaverage();

//ADC interrupt handler
interrupt (ADC12_VECTOR) adc_newresult(void);

//ADC initialisation code
void adc_init();

#endif
