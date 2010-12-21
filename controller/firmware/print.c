#include <stdint.h>
#include "stepper.h"
#include "printhead.h"
#include "print.h"

volatile printstate_t printstate;

void print_init()
{
  printstate = PRINT_IDLE;
}

/* Called by stepper interrupt */
void print_process()
{
  uint8_t i = 0;

  if (printstate == PRINT_PRINTING)
    {
      for (i=0;i<K_NOZZLES;i++)	/* Fire all nozzles */
	bk_data[i] = 1;

      printhead_period();
    }
}

void print_line()
{
  printstate = PRINT_PRINTING;
  stepper_carriagepos(2500, 1500);
  stepper_carriagepos(1000, 1500);
  printstate = PRINT_IDLE;
}

void print_cleancycle()
{
  stepper_carriagepos(500, 1500);
  stepper_carriagepos(10, SPEED_SLOW);
  stepper_carriagepos(500, SPEED_SLOW);
}


