#include "buttons.h"
#include "device.h"
#include <stdint.h>

void buttons_init()
{
  P4DIR &= ~(1<<0);
  P4DIR &= ~(1<<1);

  P4REN |= (1<<0) | (1<<1);
  P4OUT |= (1<<0) | (1<<1);
}

uint8_t buttons_getstate()
{
  return ~P4IN & ((1<<0) | (1<<1));
}
