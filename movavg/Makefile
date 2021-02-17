CC = gcc
CFLAGS = -g -Wall -O3
NASM = nasm
NASMFLAGS = -f elf64 -g -F DWARF -Wall
LD = gcc
LDFLAGS = -lm

all:
	$(CC) mov_time.c -c -o mov.o $(CFLAGS)
	$(NASM) mov_avg.asm -o mov_avg.o $(NASMFLAGS)
	$(LD) -o mov_avg mov.o mov_avg.o $(LDFLAGS)
clean:
	rm -f *.o mov_avg