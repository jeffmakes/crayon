#!/usr/bin/env python
from PIL import Image
import sys

if len(sys.argv) != 2:
    print """\nConvert RGB image to 1 bit representation in C

Usage: img2c.py <image>\n\n"""
    exit()

class RLEFiles:
    def __init__(self, img_fname):
        self.img = Image.open(img_fname)
        self.w, self.h = self.img.size
        self.img = self.img.convert("RGB")
        # 2 colours
        self.img = self.img.quantize(2)

        self.header = open("image_data.h", "w")
        self.c_file = open("image_data.c", "w")
        self.n_bytes = 0
        self.row_offsets = []

        self._output_header()
        self._output_c_file()

    def _output_header(self):
        self.header.write( """#ifndef __IMAGE_DATA_H
#define __IMAGE_DATA_H
#include <stdint.h>

extern const uint16_t image_width;
extern const uint16_t image_height;
extern const uint8_t image_data[];
const uint16_t row_offsets[%i];

#endif	/* __IMAGE_DATA_H */
""" % (self.w) )

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
        self.row_offsets += [self.n_bytes]

        for x in range(self.w):
            pixel = self.img.getpixel((x,row)) & 1

            if pixel == self.runval:
                self.runlength += 1

                if self.runlength == 127:
                    "Saturated this entry -- move on"
                    self._rle_entry_out()
                    self.runlength = 0 

            elif self.runval != None:
                "Change in value..."
                self._rle_entry_out()
                self.runlength = 1

            self.runval = pixel

        if self.runlength:
            "Output the remainder of this row"
            self._rle_entry_out()

        self.c_file.write("\n");

    def _rle_entry_out(self):
        assert self.runlength < 128
        b = self.runval << 7
        b |= self.runlength
        self.c_file.write( "0x%2.2x, " % b )
        self.n_bytes += 1

    def _output_row_offsets(self):
        self.c_file.write( """const uint16_t row_offsets[%i] = { """ % (self.w) )

        for off in self.row_offsets:
            assert off < 0x10000
            self.c_file.write( "0x%2.2x, " % off )
            self.n_bytes += 2

        self.c_file.write( """ };\n""" )

rle = RLEFiles(sys.argv[1])


