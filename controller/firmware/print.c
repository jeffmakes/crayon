#include <stdint.h>
#include "stepper.h"
#include "printhead.h"
#include "print.h"
#include "image.h"
#include <stdlib.h>

/* dimensions of the nozzle test pattern*/
#define TEST_SIZE 10
/* number of passes necessary to deposit enough wax */
#define NUM_PASSES 1
/* The nozzle to use */
#define NOZZLE 0

volatile printstate_t printstate;
/* for bitmap access, mostly */
volatile uint16_t pixel_index = 0;
/* one nozzle to print them all*/
volatile uint16_t active_nozzle = 0;
/* nozzle fire function call */
void (*fptr)(void) = NULL;

volatile uint8_t * img = NULL;

void fire_all(void);
void fire_image(void);
void fire_nozzle_test(void);

void print_init()
{
  printstate = PRINT_IDLE;
  fptr  = NULL;
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
 * TODO Tricky to reset the firing function if this function is interrupted
 * 
 */
void print_nozzle_test(void) {
  uint8_t y, sq, z;
  pixel_index = 0;
  fptr = &fire_nozzle_test;
  for (y = 0; y < K_NOZZLES; y++){
    stepper_ystep(PAGE_FORWARDS, y*TEST_SIZE);
    for (sq = 0; sq < TEST_SIZE/2; sq++){
      for (z = 0; z< NUM_PASSES; z++){
        stepper_carriagepos(2500, 6000);
        printstate = PRINT_PRINTING;
        /* wait for carriage to get there */
        stepper_carriagepos(2490, 6000);
	printstate = PRINT_IDLE;
      }

    }
  }
  fptr = NULL;
}


/* this prints an image */
/* void print_image(void)  */
/* { */
/*   uint16_t w, h; */
 
/*   /\* select image to be printed - currently only default one *\/  */
/*   image_select(&w, &h, &img); */

/*   pixel_index = 0; */
/*   fptr = &fire_image; */

/*   for (uint8_t z = 0; z< NUM_PASSES; z++){ */
/*     stepper_carriagepos(2500, 1500); */
/*     printstate = PRINT_PRINTING; */
/*     /\* wait for carriage to get there *\/ */
/*     stepper_carriagepos(2500 - w, 1500); */
/*     printstate = PRINT_IDLE; */
/*   } */
  

/*     stepper_ystep(PAGE_FORWARD, y*TEST_SIZE); */
/*     for (uint8_t sq = 0; sq < TEST_SIZE/2; y++){ */
/*   } */
/*   fptr = NULL; */

/* } */



/*************************************************************
 * set per step fire action here, head movement controlled above 
 * 
 * Functions are called by the stepper ISR
 */

/* Fire all nozzles */
void fire_all(void)
{
  uint8_t i = 0;
  /* load next pixel */
  for (i=0;i<K_NOZZLES;i++) 
    bk_data[i] = 1;
}

/*  
 *  Prints nozzle test pattern
 */
void fire_nozzle_test(void)
{
  static uint8_t noz, i = 0;
  /* paranoid clearing of bk_data */
  for (i=0;i<K_NOZZLES;i++) 
    bk_data[i] = 0;
  
  pixel_index ++;
  if (pixel_index == TEST_SIZE){
    pixel_index = 0;
    noz ++;
    noz%=K_NOZZLES;
  }
  bk_data[noz] = 1;
}


/* Print currently selected image */
void fire_image(void)
{
  /* the byte we're currently printing */
  uint8_t c; 
  /* the bit in the byte we want */
  uint8_t j; 

  uint8_t i;

  /* all nozzles off everything */
  for ( i=0;i<K_NOZZLES;i++) 
    bk_data[i] = 0;


  /* just get next bit from image data, load it, print it */
  c = img[pixel_index/8];
  j = pixel_index%8;

  if (c & (0x01 < (7-j))) {
    bk_data[NOZZLE] = 1;
  } 
  /* else fir blanks */
  pixel_index ++;
}
