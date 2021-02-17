CC = gcc
CFLAGS = -g -Wall -O3 
NASM = nasm
NASMFLAGS = -f elf64 -g -F DWARF -Wall
LD = gcc
LDFLAGS = -lm

all:
	$(CC) suma_time.c -c -o suma_c.o $(CFLAGS)
	$(NASM) suma_avx.asm -o suma_avx.o $(NASMFLAGS)
	$(NASM) suma_sse.asm -o suma_sse.o $(NASMFLAGS)
	$(LD) -o suma suma_c.o suma_avx.o suma_sse.o $(LDFLAGS)
clean:
	rm -f *.o suma