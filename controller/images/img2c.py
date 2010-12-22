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

# assume all pixels that are > 128 are white, others are black
count = 0
img = ""
b = 0x00
numbytes = 0
for y in range(h):
    for x in range(w):
        b = ((b << 1) + f.getpixel((x,y)))& 0xff
        count +=1 
        if count == 8:
            # split up int single byte units for c parsing
            img += "\\x%0x"%(b)
            count = 0
            b = 0x00
            numbytes = numbytes + 1

        
# compensate for ugly ending
if count != 0:
    # shift b by the number required
    b <<= (8-count)
    img += "\\x%0x"%(b)
    count == 0
    numbytes = numbytes + 1

print "#include <stdint.h>\n"
print "/* Size of image = %s bytes */"%numbytes
print "uint16_t image_width = %d;"%w 
print "uint16_t image_height = %d;"%h 
print "const uint8_t image_data[] = \"%s\";"%img


