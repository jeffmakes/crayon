#
# Copyright (c) 2025 Jeff Gough
#
# This software is released under the MIT License.
# See the LICENSE file for full license text.
#

ARCH=msp430x437
CFLAGS := -mmcu=$(ARCH) -g -Wall -O0
# CFLAGS := -mmcu=$(ARCH) -g -Wall -O
CFLAGS += -mendup-at=main
LDFLAGS :=

CC := msp430-gcc

C_FILES := main.c lcd.c pwm.c adc.c
H_FILES := pwm.h lcd.h adc.h

main: $(C_FILES) $(H_FILES)
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $(C_FILES) 

.PHONY: clean

clean: 
	-rm -f main 

