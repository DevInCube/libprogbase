CC=gcc
CVERSION=c89
CFLAGS=-std=$(CVERSION) -Wall -Werror -pedantic-errors
LDFLAGS=-lprogbase -lm

.PHONY: compile 

compile: 
	$(CC) *.c $(CFLAGS) $(LDFLAGS) -o program

