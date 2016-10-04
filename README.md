# libprogbase

[Repository home](https://github.com/PublicHadyniak/libprogbase)

## Installation and linkage

Clone this repository, cd to root directory and execute:
~~~~
sudo make install
~~~~

Include header files from `include/` directory and compile your code with linker option `-lprogbase`:
~~~~
gcc main.c -lprogbase
~~~~

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
