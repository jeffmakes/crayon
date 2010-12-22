#include <stdint.h>
#include "stepper.h"
#include "printhead.h"
#include "print.h"
#include "image.h"
#include <stdlib.h>

/* dimensions of the nozzle test pattern*/
#define TEST_SIZE_X 150
#define TEST_SIZE_Y 10
/* Open space on the side of images */
#define MARGIN 100
/* Default x position for printing (top left, we print left to right) */
#define PRINT_X_ORIGIN  1500
/* number of passes necessary to deposit enough wax */
#define NUM_PASSES 1
/* feed steps */
#define Y_STEP 5

/* one nozzle to print them all*/
#define NOZZLE 0

volatile printstate_t printstate;
/* for bitmap access, mostly */
volatile uint16_t pixel_index = 0;
/* width in pixels of current line*/
volatile uint16_t line_width = 0;
/* nozzle fire function call */
void (*fptr)(void) = NULL;

volatile uint8_t * img = NULL;

void fire_all(void);
void fire_image(void);
void fire_nozzle_test(void);


/*
 * Test if we're in margins
 */
uint8_t in_margins(void)
{
    if ((stepper_getpos() - PRINT_X_ORIGIN < MARGIN) || 
	(stepper_getpos() - (PRINT_X_ORIGIN + line_width+MARGIN) >= 0)) {
	/* we're in the margins */
	return 1;
    }
    else {
	return 0;
    }
}

void print_init()
{
    printstate = PRINT_IDLE;
    fptr    = NULL;
}

/* Called by stepper interrupt */
void print_process()
{
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
    stepper_carriagepos(600, 1500);
    stepper_carriagepos(5, SPEED_CLEAN);
    stepper_carriagepos(600, SPEED_CLEAN);
}

/* line with all nozzles blazing */
void print_line()
{
    fptr = &fire_all;
    printstate = PRINT_PRINTING;
    stepper_carriagepos(PRINT_X_ORIGIN, 6000);
    stepper_carriagepos(PRINT_X_ORIGIN + 5*MARGIN, 6000);
    printstate = PRINT_IDLE;
    fptr = NULL;
}



/*
 * Print (x,y) = 1 x.5 mm squares using individual nozzles, with
 * .5 mm y spacing between them.    (Assuming 0.1mm stepper resolution, reality is very different
 *
 * TODO Speed might be quite high
 * TODO Tricky to reset the firing function if this function is interrupted
 * 
 */
void print_nozzle_test(void) {
  uint8_t y, sq, z;
  uint8_t sweeps = 0;
  pixel_index = 0;
  fptr = &fire_nozzle_test;
  line_width = TEST_SIZE_X;
  for (y = 0; y < K_NOZZLES; y++){
    stepper_bodge_ystep(PAGE_FORWARDS, TEST_SIZE_Y/2*Y_STEP);
    for (sq = 0; sq < TEST_SIZE_Y/2; sq++){
      for (z = 0; z < NUM_PASSES; z++){
	if (++sweeps == 10)
	  {
	    sweeps = 0;
	    print_cleancycle();
	  }
	stepper_carriagepos(PRINT_X_ORIGIN, 1500);
	printstate = PRINT_PRINTING;
	/* wait for carriage to get there */
	stepper_carriagepos(PRINT_X_ORIGIN+TEST_SIZE_X+2*MARGIN, SPEED_SLOW);
	printstate = PRINT_IDLE;
      }
      stepper_bodge_ystep(PAGE_FORWARDS, Y_STEP);
    }
  }
  fptr = NULL;
}


/* this prints an image */
void print_image(void)    
{ 
    uint16_t w, h, prev, z, y; 
    fptr = &fire_image; 

    /* select image to be printed - currently only default one */   
    image_select(&w, &h, &img);
    line_width = w;

    pixel_index = 0; 
    for (y = 0; y < h; y ++) {
        prev = pixel_index;
        for (z = 0; z< NUM_PASSES; z++){ 
            pixel_index = prev;
            stepper_carriagepos(PRINT_X_ORIGIN, 1500); 
            printstate = PRINT_PRINTING; 
            /* wait for carriage to get there */ 
            stepper_carriagepos(PRINT_X_ORIGIN + w + MARGIN*2 , 1500); 
            printstate = PRINT_IDLE; 
        } 
 
        stepper_ystep(PAGE_FORWARDS, Y_STEP); 
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
    uint8_t i = 0;
    /* load next pixel */
    for (i=0;i<K_NOZZLES;i++) 
        bk_data[i] = 1;
}

/*    
 *    Prints nozzle test pattern
 */
void fire_nozzle_test(void)
{
    static uint8_t noz;
    uint8_t i = 0;


    if (in_margins()) {
    	return;
    }

    /* paranoid clearing of bk_data */
    for (i=0;i<K_NOZZLES;i++) 
        bk_data[i] = 0;
    
    pixel_index ++;
    if (pixel_index >= TEST_SIZE_X*NUM_PASSES){
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

    if (in_margins()) {
	return;
    }

    /* all nozzles off everything */
    for ( i=0;i<K_NOZZLES;i++) 
        bk_data[i] = 0;

    /* just get next bit from image data, load it, print it */
    c = img[pixel_index/8];
    j = pixel_index%8;

    if (c & (0x01 << (7-j))) {
        bk_data[NOZZLE] = 1;
    } 
    /* else fire blanks */
    pixel_index ++;
}
