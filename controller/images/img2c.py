#!/usr/bin/env python
from PIL import Image
import sys

if len(sys.argv) != 2:
    print """\nConvert RGB image to 1 bit representation in C

Usage: img2c.py <image>\n\n"""
    exit()

header = open("image_data.h", "w")
c_file = open("image_data.c", "w")

f = Image.open(sys.argv[1])
w,h = f.size
# 2 colours
f = f.convert("RGB")
f = f.quantize(2)

numbytes = 0
ncols = h/8
if h%8:
    ncols += 1

header.write( """#ifndef __IMAGE_DATA_H
#define __IMAGE_DATA_H
#include <stdint.h>

extern const uint16_t image_width;
extern const uint16_t image_height;
extern const uint8_t image_data[];

#endif	/* __IMAGE_DATA_H */
""" )

c_file.write( """const uint16_t image_width = %d;
const uint16_t image_height = %d;

const uint8_t image_data[] = {
""" % (w, h) )

def rle_entry_out( f, length, val ):
    assert length < 128
    b = val << 7
    b |= length
    c_file.write( "0x%2.2x, " % b )

for y in range(h):
    runval = None
    runlength = 0

    for x in range(w):
        pixel = f.getpixel((x,y)) & 1

        if pixel == runval:
            runlength += 1

            if runlength == 127:
                "Saturated this entry -- move on"
                rle_entry_out( c_file, runlength, runval )
                runlength = 0 

        elif runval != None:
            "Change in value..."
            rle_entry_out( c_file, runlength, runval )
            runlength = 1

        runval = pixel

    if runlength:
        "Output the remainder of this row"
        rle_entry_out( c_file, runlength, runval )

        runlength = 0

    c_file.write("\n");

c_file.write( "};\n" )
c_file.close()
header.close()


