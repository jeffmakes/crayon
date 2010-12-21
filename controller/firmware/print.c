#include <stdint.h>
#include "stepper.h"
#include "printhead.h"
#include "print.h"

volatile printstate_t printstate;
uint16_t (*fptr)(void) = NULL;

void print_init()
{
  printstate = PRINT_IDLE;
  fptr  = NULL
}

/* Called by stepper interrupt */
void print_process()
{
  uint8_t i = 0;
  /* only do this if fptr is defined */
  if ((printstate == PRINT_PRINTING) && (fptr))
    {
      printhead_period();
    }
}

void print_cleancycle()
{
  stepper_carriagepos(500, 1500);
  stepper_carriagepos(10, SPEED_SLOW);
  stepper_carriagepos(500, SPEED_SLOW);
}

/* line with all nozzles blazing */
void print_line()
{
  fptr = &print_fireall;
  printstate = PRINT_PRINTING;
  stepper_carriagepos(2500, 1500);
  stepper_carriagepos(1000, 1500);
  printstate = PRINT_IDLE;
  fptr = NULL;
}


/* set per step fire action here, head movement controlled above */

/* Fire all nozzles */
void print_fireall(void)
{
  /* load next pixel */
  for (i=0;i<K_NOZZLES;i++) 
    bk_data[i] = 1;
}

/*  
 *  Prints a number of 1mm squares to test nozzles
 */
void print_nozzle_test(void)
{
}
