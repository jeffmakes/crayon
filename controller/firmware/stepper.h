#ifndef __STEPPER_H
#define __STEPPER_H

void stepper_init();
void stepper_enable(uint8_t motor);
void stepper_disable(uint8_t motor);
void stepper_step(uint8_t motor, uint8_t direction);

#endif
