all: base

base: base.o
	gcc -m32 -g -Wall  -o base base.o

base.o: base.c
	gcc -m32 -g -Wall  -c -o base.o base.c

.PHONY:clean

clean:
	rm -f *.o base