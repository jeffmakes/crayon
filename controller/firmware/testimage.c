/*
 * Test image.c
 */


#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "image.h"


void main() {
    uint16_t w, h;
    uint8_t *data;
    uint8_t pixel, i;
    uint16_t count=0;

    image_print(&w, &h, &data);
    printf("%d %d\n", w,h);

    count = 0;
    for (int c=0; c < w*h/8; c++){ 
        for (i=0; i < 8; i++) {
            count ++;
            pixel = data[c] & (0x01 << (7-i));
            if (pixel == 0) {
                printf(" ");
            } else {
                printf("#");
            }
            if (count % w == 0) {
                printf("\n");
            }

        }
    }
    
}
