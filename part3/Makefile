all: menu

menu: menu.o ../base.o
	gcc -m32 -g -Wall -o menu menu.o ../base.o

menu.o: menu.c ../base.h
	gcc -m32 -g -Wall -c menu.c -o menu.o -I ..

../base.o: ../base.c ../base.h
	gcc -m32 -g -Wall -c ../base.c -o ../base.o

clean:
	rm -f *.o ../base.o menu
