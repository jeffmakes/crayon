#ifndef __PRINT_H
#define __PRINT_H

typedef enum{PRINT_IDLE = 0, PRINT_PRINTING} printstate_t;

extern volatile printstate_t printstate;

void print_init();
void print_process();
void print_line();
#endif
