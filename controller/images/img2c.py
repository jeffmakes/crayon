#!/usr/bin/env python
from PIL import Image
import sys

if len(sys.argv) < 2 or len(sys.argv) > 3:
    print """\nConvert RGB image to 1 bit representation in C

Usage: img2c.py <image> [N_BITS]\n\n"""
    exit()

N_BITS = 8
if len(sys.argv) == 3:
    N_BITS = int(sys.argv[2])
    if N_BITS not in [8,16]:
        print "N_BITS can only be 8 or 16"
        exit()

class RLEFiles:
    def __init__(self, img_fname, nbits):
        self.img = Image.open(img_fname)
        self.w, self.h = self.img.size
        self.img = self.img.convert("RGB")
        # 2 colours
        self.img = self.img.quantize(2)

        self.header = open("image_data.h", "w")
        self.c_file = open("image_data.c", "w")
        self.n_bytes = 0
        # Keys are offsets, values are the data
        self.row_data = {}
        self.row_offsets = []

        self.nbits = nbits
        # Maximum runlength
        self.MAX_RUNLEN = (1<<(self.nbits-1)) - 1

        self._output_header()
        self._output_c_file()

    def _output_header(self):
        self.header.write( """#ifndef __IMAGE_DATA_H
#define __IMAGE_DATA_H
#include <stdint.h>

#define IMAGE_N_BITS %i
extern const uint16_t image_width;
extern const uint16_t image_height;
extern const uint%i_t image_data[];
const uint16_t row_offsets[%i];

#endif	/* __IMAGE_DATA_H */
""" % (self.nbits, self.nbits, self.w) )

    def _output_c_file(self):
        self._output_c_start()

        self.c_file.write( """const uint8_t image_data[] = {\n""" )
        for y in range(0,self.h):
            self._output_c_row(y)
        self.c_file.write( """};\n""" )

        self._output_row_offsets()
        print self.n_bytes, "bytes used"

    def _output_c_start(self):
        self.c_file.write( """#include "image_data.h"

const uint16_t image_width = %d;
const uint16_t image_height = %d;
""" % (self.w, self.h) )

    def _output_c_row(self, row):
        self.runval = None
        self.runlength = 0
        data = []

        for x in range(self.w):
            pixel = self.img.getpixel((x,row)) & 1

            if pixel == self.runval:
                self.runlength += 1

                if self.runlength == self.MAX_RUNLEN:
                    "Saturated this entry -- move on"
                    data += [ self._rle_entry() ]
                    self.runlength = 0 

            elif self.runval != None:
                "Change in value..."
                data += [ self._rle_entry() ]
                self.runlength = 1

            self.runval = pixel

        if self.runlength:
            "Output the remainder of this row"
            data += [ self._rle_entry() ]

        # Has there already been a row with this value?
        for offset, d in self.row_data.iteritems():
            if d == data:
                "Identical row already output: point to that"
                self.row_offsets.append(offset)
                return

        # No identical rows found.
        # Add to library
        self.row_data[self.n_bytes] = data

        self.row_offsets.append(self.n_bytes)
        self._bytes_out(data)
        self.c_file.write("\n");

    def _rle_entry(self):
        "Returns the current RLE entry"
        assert self.runlength <= self.MAX_RUNLEN
        b = self.runval << (self.nbits - 1)
        b |= self.runlength
        return b

    def _bytes_out(self, bytes):
        for b in bytes:
            if self.nbits == 8:
                self.c_file.write( "0x%2.2x, " % b )
            elif self.nbits == 16:
                self.c_file.write( "0x%4.4x, " % b )

        self.n_bytes += len(bytes) * (self.nbits/8)

    def _output_row_offsets(self):
        self.c_file.write( """const uint16_t row_offsets[%i] = { """ % (self.w) )

        for off in self.row_offsets:
            assert off < 0x10000
            self.c_file.write( "0x%2.2x, " % off )
            self.n_bytes += 2

        self.c_file.write( """ };\n""" )

rle = RLEFiles(sys.argv[1], N_BITS)


