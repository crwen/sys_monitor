CC = /usr/bin/gcc
# CFLAGS = -Wall -g -O2 -Werror -std=gnu99
CFLAGS = -Wall -g -O2 -std=gnu99

SRC = ./src

EXE = program

CODE =  ./src/hardware/cpu/sram.c ./src/hardware/cpu/mmu.c\
./src/hardware/memory/instruction.c ./src/hardware/memory/dram.c \
./src/hardware/disk/code.c \
./src/main.c

.PHONY: program
main:
	$(CC) $(CFLAGS) -I$(SRC) $(CODE) -o $(EXE)
	./$(EXE)
clean:
	rmm -f *.o