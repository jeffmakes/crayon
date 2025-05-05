/*
 * Copyright (c) 2025 Jeff Gough
 *
 * This software is released under the MIT License.
 * See the LICENSE file for full license text.
 */

#ifndef __IMAGE_DATA_H
#define __IMAGE_DATA_H
#include <stdint.h>

#define IMAGE_N_BITS 8
extern const uint16_t image_width;
extern const uint16_t image_height;
extern const uint8_t image_data[];
const uint16_t row_offsets[682];

#endif	/* __IMAGE_DATA_H */
