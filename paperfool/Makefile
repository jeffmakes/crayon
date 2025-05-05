ARCH=msp430x2003
CFLAGS := -mmcu=$(ARCH) -g -Wall 
#CFLAGS := -mmcu=$(ARCH) -g -Wall -O
CFLAGS += -mendup-at=main
LDFLAGS := 

CC := msp430-gcc

C_FILES := main.c
H_FILES := 

main: $(C_FILES) $(H_FILES)
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $(C_FILES) 

.PHONY: clean

clean: 
	-rm -f main 

