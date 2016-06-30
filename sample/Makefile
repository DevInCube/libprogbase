CC=gcc
CVERSION=c89
CFLAGS=-std=$(CVERSION) -Wall -Werror -pedantic-errors
LDFLAGS=-lprogbase -lm

%: 
	$(CC) $@.c $(CFLAGS) $(LDFLAGS) -o $@

