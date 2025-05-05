C_FILES := getpixeltest.c ../firmware/image_data.c
H_FILES := getpixeltest.h

getpixeltest : $(C_FILES) $(H_FILES)
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $(C_FILES) 

.PHONY: clean

clean: 
	-rm -f main 

