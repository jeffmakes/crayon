#include <stdio.h>
#include <stdint.h>
#include "getpixeltest.h"

/* first pixel is MSB */
uint8_t image_getpixel(uint16_t x, uint16_t y)
{
  uint32_t index;
  index = (x + (uint16_t)y*(uint16_t)image_width);
  return image_data[ index/8 ] & (1 << (7 - index%8 ));
}

int main()
{
  uint16_t x,y;
  /* note the output image is correct. it looks slightly funky, but look in GIMP and you'll see a transparent 0th column */
  for (y=0; y<image_height; y++)
    {
      for (x=0; x<image_width; x++)
	{
	  if (image_getpixel(x, y))
	    printf("#");
	  else
	    printf("_");
	}
      printf("\n");
    }
} 

