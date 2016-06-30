#ifndef PROGBASE_H
#define PROGBASE_H

#include <stdlib.h>
#include <stdio.h>

typedef char * string;

/**
 *	you should free string after usage with freeString function
 */
string getString(void);
/**
 *	free the string from getString function
 */
void freeString(string);

int getInt(void);
long getLong(void);
float getFLoat(void);
double getDouble(void);

#endif
