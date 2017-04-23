# libprogbase v0.3.4

[![Travis-CI][travis-badge]][travis-builds]

[Repository home][home]

## Releases

Now all releases and corresponding release notes will be published in [__Releases__][releases] section

## New in v0.3.4

* New type `Ssl` and function `TcpClient_connectSecure()` in `progbase/net` module for secure connections.
    __Note__ that it is important to install `libssl-dev` linux package before installing this version of library to use this security features.

## New in v0.3.3

* New function `Console_setCursorAttribute()` in `progbase/console` module

## New in v0.3.2

* Alternative `progbase/console` module interface added

## New in v0.3.1

* `make version` now defines version of __installed__ library.
* You can use new macro `PROGBASE_VERSION_CHECK(maj, min, patch)` from `<progbase.h>` to check your code compatibility with installed library. Example:

  	```c
  	#include <progbase.h>

	#ifdef PROGBASE_VERSION
		#if !PROGBASE_VERSION_CHECK(0, 3, 1)
			#error "Please, update your libprogbase to version >= 0.3.1"
		#endif
	#else
		#warning "Update your libprogbase to version >= 0.3.1 to enable version check"
	#endif 

	int main(void) {
		puts("My libprogbase version: " PROGBASE_VERSION);
		return 0;
	} 
  	```

## New in v0.3.0

* New module `progbase/thread` for multithreading. Includes __Thread__ and __Mutex__ types.
* New module `progbase/list` with generic data-type `List` collection.
* New module `progbase/events` for event-driven development. Added samples for `progbase/events`.
* New function in `progbase/net` module:

  `Ip_resolveHostname()` to convert hostname to IP-address (`progbase.herokuapp.com -> 54.217.217.104`):

  ```c
  bool Ip_resolveHostname(char * ipv4, const char * hostname);
  ```

## New in v0.2.0

* Added new module `progbase/net` for TCP and UDP networking.
* `pbconsole.h` became deprecated, use `progbase/console.h` instead.
* Now `make install` uses __cmake__ and `CMakeLists.txt` to build and install library.

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

```c
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
```


[home]: https://github.com/PublicHadyniak/libprogbase
[releases]: https://github.com/PublicHadyniak/libprogbase/releases
[travis-badge]: https://travis-ci.org/PublicHadyniak/libprogbase.svg?branch=master
[travis-builds]: https://travis-ci.org/PublicHadyniak/libprogbase/builds
