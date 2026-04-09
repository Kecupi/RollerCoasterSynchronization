CC=gcc
CFLAGS=-std=gnu99 -Wall -Werror -pedantic

all: proj2

proj2.o: proj2.c

proj2: proj2.o
    $(CC) $(CFLAGS) $^ -o $@

zip:
    zip xhorens00.zip proj2.c Makefile

clean:
    rm *.o *.zip