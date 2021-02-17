CC = gcc
CFLAGS = -g -Wall -O3 
NASM = nasm
NASMFLAGS = -f elf64 -g -F DWARF -Wall
LD = gcc
LDFLAGS = -lm

all:
	$(CC) cociente_time.c -c -o cociente_c.o $(CFLAGS)
	$(NASM) cociente_avx.asm -o cociente_avx.o $(NASMFLAGS)
	$(NASM) cociente_sse.asm -o cociente_sse.o $(NASMFLAGS)
	$(LD) -o cociente cociente_c.o cociente_avx.o cociente_sse.o $(LDFLAGS)
clean:
	rm -f *.o cociente