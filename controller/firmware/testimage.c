/*
 * Test image.c
 */


#include <stdint.h>
#include <stdlib.h>
#include "image.h"


void main() {
    uint16_t w, h;
    char data[];
    image_print(&w, &h, data);

    for (int i=0; i < w*h; i++){ 
	printf("%0x",data[i]);
    }
}
