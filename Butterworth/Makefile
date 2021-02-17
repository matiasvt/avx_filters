CC = gcc
CFLAGS = -g -Wall -O3
NASM = nasm
NASMFLAGS = -f elf64 -g -F DWARF -Wall
LD = gcc
LDFLAGS = -lm

all:
	$(CC) time_buter.c -c -o buter.o $(CFLAGS)
	$(NASM) buter.asm -o buter_a.o $(NASMFLAGS)
	$(LD) -o buter buter.o buter_a.o $(LDFLAGS)
clean:
	rm -f *.o buter