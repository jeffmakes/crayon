#ifndef __IMAGE_H
#define __IMAGE_H
#include <stdint.h>
#include "image_data.h"
/* 
 * set up variables to point to image data
 * TODO select different images
 *
 * Return 
 * - w, h in pixels
 * - pointer to image data
 */

void image_select(uint16_t *w, uint16_t *h, volatile uint8_t ** data);
uint8_t image_getpixel(uint16_t x, uint16_t y) ;

#endif
