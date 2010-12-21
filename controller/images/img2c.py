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
#print f.tostring()


# assume all pixels that are > 128 are white, others are black
count = 0
img = ""
b = 0xff
for y in range(h):
    for x in range(w):
        b = ((b << 1) + f.getpixel((x,y)))& 0xff
        count +=1 
        if count == 8:
            # split up int single byte units for c parsing
            img += "\\x%x"%(b)
            count = 0
            b = 0xff

        
# compensate for ugly ending
if count != 0:
    # shift b by the number required
    b << (8-count)
    img += chr(b)
    count == 0

print "uint16_t image_width = %d;"%w 
print "uint16_t image_width = %d;"%h 
print "static image_data[] = \"%s\";"%img
f.show()


