/*
 * Copyright (c) 2025 Jeff Gough
 *
 * This software is released under the MIT License.
 * See the LICENSE file for full license text.
 */

#ifndef __DEVICE_H
#define __DEVICE_H

#include <msp430x261x.h>

/* define PORT A registers */
sfrw(PAREN, 0x0014);
sfrw(PASEL, 0x003E);
sfrw(PADIR, 0x003C);
sfrw(PAOUT, 0x003A);
sfrw(PAIN,  0x0038);

#endif

