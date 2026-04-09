CC=gcc
CFLAGS=-std=gnu99 -Wall -Werror -pedantic

all: proj2

proj2.o: proj2.c proj2_input.h
proj2_input.o: proj2_input.c proj2_input.h

proj2: proj2.o proj2_input.o
	$(CC) $(CFLAGS) $^ -o $@

zip:
	zip xhorens00.zip proj2.c Makefile

clean:
	rm *.o *.zip proj2