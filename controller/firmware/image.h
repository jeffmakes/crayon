/*
 * Copyright (c) 2025 Jeff Gough
 *
 * This software is released under the MIT License.
 * See the LICENSE file for full license text.
 */

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

uint8_t image_getpixel(uint16_t x, uint16_t y) ;

#endif
