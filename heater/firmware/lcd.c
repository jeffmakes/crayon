/*
 * Copyright (c) 2025 Jeff Gough
 *
 * This software is released under the MIT License.
 * See the LICENSE file for full license text.
 */

#include <msp430x43x.h>

unsigned char *lcd = LCDMEM; //pointer to lcd memory map

const unsigned char digit[40] =
  {
    0x11, 0x11,  // "0"    LCD segments a+b & c+d = lower two bytes
    0x11, 0x00,  // "0"    LCD segments e+f & g+h = upper two bytes
    0x10, 0x01,  // "1"
    0x00, 0x00,  // "1"
    0x11, 0x10,  // "2"
    0x01, 0x01,  // "2"
    0x11, 0x11,  // "3"
    0x00, 0x01,  // "3"
    0x10, 0x01,  // "4"
    0x10, 0x01,  // "4"
    0x01, 0x11,  // "5"
    0x10, 0x01,  // "5"
    0x01, 0x11,  // "6"
    0x11, 0x01,  // "6"
    0x11, 0x01,  // "7"
    0x00, 0x00,  // "7"
    0x11, 0x11,  // "8"
    0x11, 0x01,  // "8"
    0x11, 0x11,  // "9"
    0x10, 0x01,  // "9"
  };

void lcd_init()
{
  //set up function select registers for LCD
  P5SEL = 0x03; 
  P4SEL = 0xff;
  P3SEL = 0xff;
  
  //set up port direction registers for LCD
  P5DIR = 0xff;
  P4DIR = 0xff;
  P3DIR = 0xff;
  P2DIR = 0xc0;
  
  //set up LCD drive frequency
  BTCTL = BT_fLCD_DIV256;
  //set up LCD
  LCDCTL = LCDSG0_5 | LCDSTATIC | LCDON; 
}

void lcd_clear()
{
  unsigned char i = 0;

  for (i=0; i<20; i++)
    lcd[i] = 0;
}

//display a value between 0 and 1999 on lcd
void lcd_dispvalue(unsigned int value)
{
  unsigned char twodp = 0;
  unsigned char i = 0;
  
  /*if (value<=1999)
    {
      if (value>999)
	LCDM15 = 0x11;
      else 
	LCDM15 = 0x10;
    }
  else if (value>2000)
    {
      //set leading 1 for values over 9999
      if (value>9999)
	LCDM15=0x01;
      else
	LCDM15=0x00;
      
      //divide by ten to shift digits right
      value = value / 10;
      
      twodp = 1;
      }*/

  //display units and tens
  for (i=0;i<4;i++)
    lcd[i]=digit[(value%10)*4 + i];
  for (i=0;i<4;i++)
    lcd[i+4]=digit[((value/10)%10)*4 + i];
  for (i=0;i<4;i++)
    lcd[i+10]=digit[((value/100)%10)*4 + i];

  //set the second decimal point if necessary
  //  if (twodp)
  //    LCDM14 |= 0x10;

  LCDM4 |= 0x10; //decimal point
}
