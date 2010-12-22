/* 
 * This is were static images live
 */
#include "image.h"

void image_select(uint16_t *w, uint16_t *h, volatile uint8_t ** data)
{
  *w = image_width;
  *h = image_height;
  *data = image_data;
}

/* first pixel is MSB */
uint8_t image_getpixel(uint16_t x, uint16_t y)
{
  return (image_data[y][x/8] & (1 << (7 - x%8))) ? 1:0;
}



