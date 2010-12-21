#ifndef __STEPPER_H
#define __STEPPER_H

#define SPEED_SLOW 4000
#define SPEED_NORMAL 1000

#define X_MOTOR 1
#define Y_MOTOR 0
#define CARRIAGE_LEFT 1
#define CARRIAGE_RIGHT 0
#define PAGE_FORWARDS 1
#define PAGE_BACKWARDS 0
#define STOPPED 3


#define stepper_ishome() (P6IN & (1<<5) ? 0 : 1)
void stepper_init();
void stepper_enable(uint8_t motor);
void stepper_disable(uint8_t motor);
void stepper_setxvelocity(uint16_t speed, uint8_t direction);
void stepper_carriagepos(uint16_t newpos, uint16_t speed);
void stepper_ystep( uint8_t direction, uint8_t steps);
void stepper_xhome();



#endif
