all: ELFmenu 

ELFmenu: ELFmenu.o
	gcc	-g	-m32	-Wall	-o	ELFmenu	ELFmenu.o

ELFmenu.o: ELFmenu.c
	gcc	-g	-m32	-Wall	-c	-o	ELFmenu.o	ELFmenu.c 

.PHONY: clean

clean:
	rm -f *.o ELFmenu 