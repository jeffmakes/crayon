ARCH=msp430x2619
CFLAGS := -mmcu=$(ARCH) -g -Wall 
#CFLAGS := -mmcu=$(ARCH) -g -Wall -O
CFLAGS += -mendup-at=main
LDFLAGS :=

CC := msp430-gcc

C_FILES := main.c draw.c video.c
H_FILES := draw.h device.h watchdog.h video.h

main: $(C_FILES) $(H_FILES)
	$(CC) -Wl,--section-start -Wl,.video=0x3100 -o $@ $(CFLAGS) $(LDFLAGS) $(C_FILES) 

.PHONY: clean

clean: 
	-rm -f main 

