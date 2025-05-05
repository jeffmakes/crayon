/* 
 * This is were static images live
 */
#include "image.h"

/* first pixel is MSB */
uint8_t image_getpixel(uint16_t x, uint16_t y)
{
  uint32_t index;
  index = ((uint32_t)x + (uint32_t)y * (uint32_t)image_width);
  //return !!(image_data[ index/8 ] & (1 << (7 - index%8 )));

  const uint8_t *pos = &image_data[ row_offsets[y] ];
  uint16_t xpos = 0;

  /* Work through the row until the requested position */
  while(1) {
    uint8_t runlength = (*pos) & 0x7f;

    if( xpos + runlength > x )
      /* Found the entry we want */
      break;

    xpos += runlength;
    pos++;
  }

  return ((*pos) >> 7) & 1;
}



