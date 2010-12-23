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
#define MARGIN 50
/* Default x position for printing (top left, we print left to right) */
#define PRINT_X_ORIGIN  1500
/* number of passes necessary to deposit enough wax */
#define NUM_PASSES 10
/* feed steps */
#define Y_STEP 1
#define Z_LAYERS 4

#define STARTNOZZLE 43
#define ENDNOZZLE 45

volatile printstate_t printstate;

/* nozzle fire function call */
void (*fptr)(void) = NULL;

volatile uint8_t * img = NULL;
volatile uint16_t x, y;

void print_init()
{
  uint8_t i = 0;
  
  for (i = 0; i<K_NOZZLES; i++)
    bk_data[i] = 0;
  printstate = PRINT_IDLE;
  fptr    = NULL;
  x = 0;
  y = 0;
    
}

/* Called by stepper interrupt */
void print_nextpixel()
{
  uint8_t nozzle;
  uint16_t currentpos;
  uint8_t pixelwhite;
  uint8_t zcount;

  currentpos = stepper_getXpos(); 
  if ( (currentpos >= PRINT_X_ORIGIN + MARGIN)
       && (currentpos < PRINT_X_ORIGIN + MARGIN + image_width)) /* If we're within the printing zone... */
    {
      x = currentpos - (PRINT_X_ORIGIN + MARGIN);
      if (printstate == PRINT_PRINTING)
	{  
	  pixelwhite = image_getpixel(x, y);
	  for (nozzle = STARTNOZZLE; nozzle < ENDNOZZLE; nozzle++)
	    bk_data[nozzle] = pixelwhite;

	  //	  for (zcount = 0; zcount<Z_LAYERS; zcount++)
	  printhead_period();		/* fire the nozzles */
	}
    }
}

void print_image()
{
  printstate = PRINT_IDLE;
  stepper_moveXto(PRINT_X_ORIGIN, SPEED_PRINT);
  stepper_moveXto(PRINT_X_ORIGIN + MARGIN, SPEED_PRINT);

  printstate = PRINT_PRINTING;
  while (printstate != PRINT_FINISHED)
    {
      stepper_moveXto(PRINT_X_ORIGIN + MARGIN + image_width + MARGIN, SPEED_PRINT); /* print right */
      stepper_moveXto(PRINT_X_ORIGIN, SPEED_PRINT); /* undershoot into margin */

      if (++y == image_height)
	printstate = PRINT_FINISHED;
      else
	stepper_Ystep(PAGE_FORWARDS, Y_STEP); /* advance the page */
    }
}

void print_cleancycle()
{
    printstate = PRINT_IDLE;
    stepper_moveXto(600, 1500);
    stepper_moveXto(5, SPEED_CLEAN);
    stepper_moveXto(600, SPEED_CLEAN);
}

/*
 * Print (x,y) = 1 x.5 mm squares using individual nozzles, with
 * .5 mm y spacing between them.    (Assuming 0.1mm stepper resolution, reality is very different
 *
 * TODO Speed might be quite high
 * TODO Tricky to reset the firing function if this function is interrupted
 * 
 */
/* void print_nozzle_test(void) { */
/*   uint8_t y, sq, z; */
/*   uint8_t sweeps = 0; */
/*   pixel_index = 0; */
/*   fptr = &fire_nozzle_test; */
/*   line_width = TEST_SIZE_X; */
/*   for (y = 0; y < K_NOZZLES; y++){ */
/*     stepper_Ystep(PAGE_FORWARDS, TEST_SIZE_Y/2*Y_STEP); */
/*     for (sq = 0; sq < TEST_SIZE_Y/2; sq++){ */
/*       for (z = 0; z < NUM_PASSES; z++){ */
/* 	if (++sweeps == 10) */
/* 	  { */
/* 	    sweeps = 0; */
/* 	    print_cleancycle(); */
/* 	  } */
/* 	stepper_moveXto(PRINT_X_ORIGIN, 1500); */
/* 	printstate = PRINT_PRINTING; */
/* 	/\* wait for carriage to get there *\/ */
/* 	stepper_moveXto(PRINT_X_ORIGIN+TEST_SIZE_X+2*MARGIN, SPEED_SLOW); */
/* 	printstate = PRINT_IDLE; */
/*       } */
/*       stepper_Ystep(PAGE_FORWARDS, Y_STEP); */
/*     } */
/*   } */
/*   fptr = NULL; */
/* } */


/* this prints an image */
/* void print_image(void)     */
/* {  */
/*     uint16_t w, h, prev, z, y;  */
/*     fptr = &fire_image;  */

/*     /\* select image to be printed - currently only default one *\/    */
/*     image_select(&w, &h, &img); */
/*     line_width = w; */

/*     pixel_index = 0;  */
/*     for (y = 0; y < h; y ++) { */
/*         prev = pixel_index; */
/*         for (z = 0; z< NUM_PASSES; z++){  */
/*             pixel_index = prev; */
/*             stepper_moveXto(PRINT_X_ORIGIN, 1500);  */
/*             printstate = PRINT_PRINTING;  */
/*             /\* wait for carriage to get there *\/  */
/*             stepper_moveXto(PRINT_X_ORIGIN + w + MARGIN*2 , 1500);  */
/*             //printstate = PRINT_IDLE;  */
/*         }  */
 
/*         stepper_Ystep(PAGE_FORWARDS, Y_STEP);  */
/*     } */
/*     fptr = NULL;  */
/* }  */



/* /\************************************************************* */
/*  * set per step fire action here, head movement controlled above  */
/*  *  */
/*  * Functions are called by the stepper ISR */
/*  *\/ */

/* /\* Fire all nozzles *\/ */
/* void fire_all(void) */
/* { */
/*     uint8_t i = 0; */
/*     /\* load next pixel *\/ */
/*     for (i=0;i<K_NOZZLES;i++)  */
/*         bk_data[i] = 1; */
/* } */

/* /\*     */
/*  *    Prints nozzle test pattern */
/*  *\/ */
/* void fire_nozzle_test(void) */
/* { */
/*     static uint8_t noz; */
/*     uint8_t i = 0; */


/*     if (in_margins()) { */
/*     	return; */
/*     } */

/*     /\* paranoid clearing of bk_data *\/ */
/*     for (i=0;i<K_NOZZLES;i++)  */
/*         bk_data[i] = 0; */
    
/*     pixel_index ++; */
/*     if (pixel_index >= TEST_SIZE_X*NUM_PASSES){ */
/*         pixel_index = 0; */
/*         noz ++; */
/*         noz%=K_NOZZLES; */
/*     } */
/*     bk_data[noz] = 1; */
/* } */


/* /\* Print currently selected image *\/ */
/* void fire_image(void) */
/* { */
/*     /\* the byte we're currently printing *\/ */
/*     uint8_t c;  */
/*     /\* the bit in the byte we want *\/ */
/*     uint8_t j;  */

/*     uint8_t i; */

/*     if (in_margins()) { */
/* 	return; */
/*     } */

/*     /\* all nozzles off everything *\/ */
/*     //    for ( i=0;i<K_NOZZLES; */
/*     //bk_data[i] = 0; */

/*     /\* just get next bit from image data, load it, print it *\/ */
/*     c = img[pixel_index/8]; */
/*     j = pixel_index%8; */

/*     if (c & (0x01 << (7-j))) */
/*       { */
/* 	bk_data[40] = 1; */
/* 	bk_data[41] = 1; */
/* 	bk_data[42] = 1; */
/* 	bk_data[43] = 1; */
/* 	bk_data[44] = 1; */
/*       }  */
/*     else */
/*       { */
/* 	bk_data[40] = 0; */
/* 	bk_data[41] = 0; */
/* 	bk_data[42] = 0; */
/* 	bk_data[43] = 0; */
/* 	bk_data[44] = 0; */
/*       }  */

   
/*     /\* else fire blanks *\/ */
/*     pixel_index ++; */
/* } */
