#!/usr/bin/env python
from PIL import Image
import sys

if len(sys.argv) != 2:
    print """\nConvert RGB image to 1 bit representation in C

Usage: img2c.py <image>\n\n"""
    exit()


f = Image.open(sys.argv[1])
w,h = f.size
# 2 colours
f = f.convert("RGB")
f = f.quantize(2)

numbytes = 0

print "#include <stdint.h>\n"
print "uint16_t image_width = %d;"%w
print "uint16_t image_height = %d;"%h

ncols = h/8
if h%8:
    ncols += 1
print "const uint8_t image_data[%i][%i] = {" % (w,ncols)

for y in range(h):
    s = ""
    count = 0
    img = ""
    b = 0x00
    first = True

    for x in range(w):
        b = ((b << 1) + f.getpixel((x,y)))& 0xff
        count +=1 

        if count == 8:
            # split up int single byte units for c parsing
            if not first:
                s += ", "
            s += "0x%2.2x"%(b)

            first = False
            count = 0
            b = 0x00
            numbytes = numbytes + 1
        
    # Pad the last byte
    if count != 0:
        # shift b by the number required
        b <<= (8-count)
        if not first:
            s += ", "
        s += "0x%2.2x"%(b)
        count == 0
        numbytes = numbytes + 1

    if y == h-1:
        print "\t{%s}" % s
    else:
        print "\t{%s}," % s

print "};"


