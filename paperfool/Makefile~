ARCH=msp430x249
CFLAGS := -mmcu=$(ARCH) -g -Wall 
#CFLAGS := -mmcu=$(ARCH) -g -Wall -O
CFLAGS += -mendup-at=main
LDFLAGS := 

CC := msp430-gcc

C_FILES := main.c stepper.c draw.c font.c
H_FILES := stepper.h draw.h font.h

main: $(C_FILES) $(H_FILES)
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $(C_FILES) 

.PHONY: clean

clean: 
	-rm -f main 

