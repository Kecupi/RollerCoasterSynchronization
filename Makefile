CC=gcc
CFLAGS=-std=gnu99 -Wall -Werror -pedantic
LDLIBS=-lpthread -lrt

all: proc_main

proc_main.o: proc_main.c proc_input.h proc_entities.h
proc_input.o: proc_input.c proc_input.h proc_entities.h
proc_entities.o: proc_entities.c proc_entities.h

proc_main: proc_main.o proc_input.o proc_entities.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS)

test:
	cd tests && python3 tests.py

zip:
	zip xhorens00.zip proc_main.c proc_input.c proc_input.h proc_entities.c proc_entities.h Makefile

clean:
	rm -f *.o *.zip log.out proc_main