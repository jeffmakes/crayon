ARCH=msp430x2618
CFLAGS := -mmcu=$(ARCH) -g -Wall 
#CFLAGS := -mmcu=$(ARCH) -g -Wall -O
CFLAGS += -mendup-at=main
LDFLAGS :=

CC := msp430-gcc

C_FILES := main.c wavegen.c printhead.c stepper.c print.c
H_FILES := wavegen.h device.h printhead.h stepper.h print.h

main: $(C_FILES) $(H_FILES)
	$(CC) -Wl,--section-start -Wl,.video=0x3100 -o $@ $(CFLAGS) $(LDFLAGS) $(C_FILES) 

.PHONY: clean

testimage: image.c image.h testimage.c
	gcc -std=c99 testimage.c image.c -o testimage

clean: 
	-rm -f main 

