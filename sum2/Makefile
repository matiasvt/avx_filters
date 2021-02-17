CC = gcc
CFLAGS = -g -Wall -O3 
NASM = nasm
NASMFLAGS = -f elf64 -g -F DWARF -Wall
LD = gcc
LDFLAGS = -lm

all:
	$(CC) suma2_time.c -c -o suma2_c.o $(CFLAGS)
	$(NASM) suma2_avx.asm -o suma2_avx.o $(NASMFLAGS)
	$(NASM) suma2_sse.asm -o suma2_sse.o $(NASMFLAGS)
	$(LD) -o suma2 suma2_c.o suma2_avx.o suma2_sse.o $(LDFLAGS)
clean:
	rm -f *.o suma2