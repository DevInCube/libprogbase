# libprogbase v0.2.0

<a href='https://travis-ci.org/PublicHadyniak/libprogbase/builds'><img src='https://travis-ci.org/PublicHadyniak/libprogbase.svg?branch=master'></a>

[Repository home](https://github.com/PublicHadyniak/libprogbase)

## New in v0.2.0

* Added new module progbase/net` for TCP and UDP networking.
* `pbconsole.h` became deprecated, use `progbase/console.h` instead.
* Now `make install` uses __cmake__ and `CMakeLists.txt` for build and install.

## New in v0.1.6

* Now you can link this library with C++ programs.
* `CMakeLists.txt` for __cmake__ compilation under __VSCode__.


## New in v0.1.5

New `pbconsole.h` functions:

~~~~c

void conLockInput(void);
void conUnlockInput(void);
~~~~

Samples:

* [conLock.c](https://github.com/PublicHadyniak/libprogbase/blob/master/sample/conLock.c)
* [conLockFire.c](https://github.com/PublicHadyniak/libprogbase/blob/master/sample/conLockFire.c)

## New in v0.1.4

New `progbase.h` function:

~~~~c
char getChar(void);
~~~~

All input functions were fixed to skip redundant characters from stdin stream after '\n' character.

Added new `.gcc-flags.json` config file for Atom gcc-linter plugin. All sources updated, some warnings and errors fixed.

## New in v0.1.3

New `pbconsole.h` functions:

~~~~c

/* functions to control cursor visibility */
void conHideCursor(void);
void conShowCursor(void);
~~~~

## New in v0.1.2

~~~~c

/* terminal cursor position */
struct conpos {
	unsigned int column;
	unsigned int row;
};
~~~~

New `pbconsole.h` functions:

~~~~c

/* get terminal cursor position */
struct conpos conGetPos(void);
~~~~

## New in v0.1.1

New `pbconsole.h` type:

~~~~c

/* terminal size data */
struct consize {
	unsigned short cols;
	unsigned short rows;
};
~~~~

New `pbconsole.h` functions:

~~~~c

/* detect if some key is down (non-blocking) */
int conIsKeyDown(void);

/* get key down code without pressing Enter key */
char conGetChar(void);  

/* get cols and rows terminal size structure */
struct consize conGetSize(void);  

/* set terminal cols and rows size */
void conResize(unsigned short rows, unsigned short cols);  
~~~~

New `pbconsole.h` constants:

~~~~c

#define CON_MIN_ROWS 5
#define CON_MIN_COLS 29
~~~~

Samples:

* [conIsKeyDown.c](https://github.com/PublicHadyniak/libprogbase/blob/master/sample/conIsKeyDown.c)
* [conGetChar.c](https://github.com/PublicHadyniak/libprogbase/blob/master/sample/conGetChar.c)
* [conResize.c](https://github.com/PublicHadyniak/libprogbase/blob/master/sample/conResize.c)

## Installation and linkage

Clone this repository with command:
~~~~
git clone https://github.com/PublicHadyniak/libprogbase.git
~~~~

Change directory to newly created `libprogbase/` directory and execute this command:
~~~~
sudo make install
~~~~

Include header files from `include/` directory and compile your code with linker option `-lprogbase`:
~~~~
gcc main.c -lprogbase
~~~~

## Check library version (since 0.1.0)

You need to change directory to cloned `libprogbase/` directory and execute command:
~~~~
make version
~~~~

## Library update

### Auto (since 0.1.0)

Works only if you cloned this repository with Git.

Change directory to cloned `libprogbase/` and execute command:
~~~~
sudo make update
~~~~

And that's all.

### Manual

You need to re-install library.

## Headers

### progbase.h

Sample usage to read all available data type values from terminal:
~~~~c
#include <stdio.h>
#include <stdlib.h>
#include <progbase.h>

int main(void) {
	int i = 0;
	long l = 0;
	float f = 0;
	double d = 0;
	string s = "";

	printf("Enter int: ");
	i = getInt();
	printf("Entered: %d\n", i);

	printf("Enter long: ");
	l = getLong();
	printf("Entered: %ld\n", l);

	printf("Enter float: ");
	f = getFloat();
	printf("Entered: %f\n", f);

	printf("Enter double: ");
	d = getDouble();
	printf("Entered: %lf\n", d);

	printf("Enter string: ");
	s = getString();
	printf("Entered: %s\n", s);

	freeString(s);
	return 0;
}
~~~~

Sleep milliseconds sample `sleepMillis()` (since 0.1.0):
~~~~c
#include <stdio.h>
#include <progbase.h>

int main(void) {
	int i = 0;
	const unsigned long MILLIS = 100;

	for (i = 0; i < 100; i++) {
		if (0 == i % 10) {
			puts("");
		}
		printf("%i\t", i);
		fflush(stdout);
		sleepMillis(MILLIS);
	}
	puts("");
	return 0;
}
~~~~
