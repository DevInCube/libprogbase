CC=gcc
LOCALDIR=/usr/local

install: headers libprogbase

headers:
	echo "copying headers..."
	cp include/*.h $(LOCALDIR)/include/

libprogbase:
	echo "building library..."
	gcc -c src/*.c -Iinclude
	ar rcs $@.a *.o
	echo "copying library..."
	mv $@.a $(LOCALDIR)/lib/$@.a
	rm *.o

