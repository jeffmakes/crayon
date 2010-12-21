#ifndef __STEPPER_H
#define __STEPPER_H

#define SPEED_SLOW 4000
#define SPEED_NORMAL 1000

#define stepper_ishome() (P6IN & (1<<5) ? 0 : 1)
void stepper_init();
void stepper_enable(uint8_t motor);
void stepper_disable(uint8_t motor);
void stepper_step(uint8_t motor, uint8_t direction);
void stepper_setxvelocity(uint16_t speed, uint8_t direction);
void stepper_setdir(uint8_t motor, uint8_t direction);
void stepper_carriagepos(uint16_t newpos, uint16_t speed);


#endif
