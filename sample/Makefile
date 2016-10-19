CC=gcc
CVERSION=c89
CFLAGS=-std=$(CVERSION) -Wall -Werror -pedantic-errors
LDFLAGS=-lprogbase -lm

.PHONY: compile

compile:
	$(CC) main.c $(CFLAGS) $(LDFLAGS) -o program
