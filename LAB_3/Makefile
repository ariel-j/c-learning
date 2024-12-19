# all: task0 task1 task2
all: task0

task0: start.o main.o Util.o
	ld -m elf_i386 start.o main.o Util.o -o task0

# task1: start.o task1.o
# 	ld -m elf_i386 start.o task1.o -o task1

# task2: start.o task2.o Util.o
# 	ld -m elf_i386 start.o task2.o Util.o -o task2

start.o: start.s
	nasm -f elf32 start.s -o start.o

Util.o: Util.c
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector Util.c -o Util.o

main.o: main.c
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector main.c -o main.o

# task1.o: task1.s
# 	nasm -f elf32 task1.s -o task1.o

# task2.o: task2.c
# 	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector task2.c -o task2.o

.PHONY: clean
clean:
	rm -f *.o task0 task1 task2