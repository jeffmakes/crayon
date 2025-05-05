/*
 * Copyright (c) 2025 Jeff Gough
 *
 * This software is released under the MIT License.
 * See the LICENSE file for full license text.
 */

#ifndef __STEPPER_H
#define __STEPPER_H

#define SPEED_SLOW 4000
#define SPEED_CLEAN 8000
#define SPEED_NORMAL 1000
#define SPEED_PRINT 1200

#define X_MOTOR 1
#define Y_MOTOR 0
#define CARRIAGE_LEFT 1
#define CARRIAGE_RIGHT 0
#define PAGE_FORWARDS 0
#define PAGE_BACKWARDS 1
#define STOPPED 3

extern volatile uint16_t carriagepos;

#define stepper_ishome() (P6IN & (1<<5) ? 0 : 1)
void stepper_init();
void stepper_enable(uint8_t motor);
void stepper_disable(uint8_t motor);
void stepper_setxvelocity(uint16_t speed, uint8_t direction);
void stepper_moveXto(uint16_t newpos, uint16_t speed);
void stepper_Ystep( uint8_t direction, uint8_t steps);

void stepper_xhome();
uint16_t stepper_getXpos();



#endif
