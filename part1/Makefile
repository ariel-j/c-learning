all: encoder

my_encoder: encoder.o
	gcc -m32 -g -Wall -o encoder encoder.o

my_encoder.o: my_encoder.c
	gcc -m32 -g -Wall -c encoder.c

.PHONY: clean
clean:
	rm -f *.o encoder
