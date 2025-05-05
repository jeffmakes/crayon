/*
 * Copyright (c) 2025 Jeff Gough
 *
 * This software is released under the MIT License.
 * See the LICENSE file for full license text.
 */

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
