#ifndef __IMAGE_H
#define __IMAGE_H
#include <stdint.h>
/* 
 * set up variables to point to image data
 * TODO select different images
 *
 * Return 
 * - w, h in pixels
 * - pointer to image data
 */

void image_select(uint16_t *w, uint16_t *h, uint8_t ** data);


#endif
