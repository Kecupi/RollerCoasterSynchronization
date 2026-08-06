CC=gcc
CFLAGS=-std=gnu99 -Wall -Werror -pedantic

all: proc_main

proc.o: proc_main.c proc_input.h
proc_input.o: proc_input.c proc_input.h

proc_main: proc.o proc_input.o
	$(CC) $(CFLAGS) $^ -o $@

zip:
	zip xhorens00.zip proc_main.c proc_input.c proc_input.h Makefile

clean:
	rm -f *.o *.zip proc_main