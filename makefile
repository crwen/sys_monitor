CC = /usr/bin/gcc
CFLAGS = -Wall -g -O2 -Werror -std=gnu99

SRC = ./src

EXE = program

CODE =  ./src/hardware/cpu/sram.c ./src/main.c

.PHONY: program
main:
	$(CC) $(CFLAGS) -I$(SRC) $(CODE) -o $(EXE)


run:
	./$(EXE)