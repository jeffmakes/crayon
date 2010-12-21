#include <stdint.h>
#include "stepper.h"
#include "printhead.h"
#include "print.h"

/* dimensions of the nozzle test pattern*/
#define TEST_SIZE 10
/* number of passes necessary to deposit enough wax */
#define NUM_PASSES = 10

volatile printstate_t printstate;
volatile uint16_t pixel_index = 0;
uint16_t (*fptr)(void) = NULL;

/*
 * Dummy function 
 */
void stepper_feedpos(uint16_t newpos, uint16_t speed) {
}


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
      /* load */
      fptr();
      /* fire */
      printhead_period();
    }
}

void print_cleancycle()
{
  printstate = PRINT_IDLE;
  stepper_carriagepos(500, 1500);
  stepper_carriagepos(10, SPEED_SLOW);
  stepper_carriagepos(500, SPEED_SLOW);
}

/* line with all nozzles blazing */
void print_line()
{
  fptr = &fire_all;
  printstate = PRINT_PRINTING;
  stepper_carriagepos(2500, 1500);
  stepper_carriagepos(1000, 1500);
  printstate = PRINT_IDLE;
  fptr = NULL;
}



/*
 * Print (x,y) = 1 x.5 mm squares using individual nozzles, with
 * .5 mm y spacing between them.  (Assuming 0.1mm stepper resolution
 *
 * TODO Speed might be quite high
 * TODO currently only using the first 45 nozzles
 */
void print_nozzle_test(void) {
  pixel_index = 0;
  fptr = &fire_nozzle_test;
  y = 0;
  for (uint8_t y = 0; y < K_NOZZLES/2; y++){
    stepper_feedpos(y*TEST_SIZE, 1500);
    for (uint8_t sq = 0; sq < TEST_SIZE/2; y++){
      for (uint8_t z = 0; z< NUM_PASSES; z++){
        stepper_carriagepos(2500, 1500);
        printstate = PRINT_PRINTING;
        /* wait for carriage to get there */
        stepper_carriagepos(2490, 1500);
      }
      printstate = PRINT_IDLE;
  }
  fptr = NULL;
}





/*************************************************************
 * set per step fire action here, head movement controlled above 
 * 
 * Functions are called by the stepper ISR
 */

/* Fire all nozzles */
void fire_all(void)
{
  /* load next pixel */
  for (i=0;i<K_NOZZLES;i++) 
    bk_data[i] = 1;
}

/*  
 *  Prints nozzle test pattern
 */
void fire_nozzle_test(void)
{
  static noz = 0;
  
  pixel_index ++
  if (pixel_index == TEST_SIZE){
    pixel_index = 0;
    bk_data[noz] = 0;
    noz ++;
    bk_data[noz] = 1;
  }
}
