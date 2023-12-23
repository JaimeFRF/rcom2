HEADERS = functions.h macros.h
OBJECTS = main.o functions.o 
CFLAGS = -Wall

%.o: %.c $(HEADERS)
	gcc $(CFLAGS) -c $< -o $@

download: $(OBJECTS)
	gcc $(CFLAGS) $(OBJECTS) -o $@

clean:
	-rm -f $(OBJECTS)
	-rm -f download
	-rm -f pub.txt
