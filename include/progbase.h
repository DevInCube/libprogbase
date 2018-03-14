/** @file
    @brief Progbase common module
*/

#pragma once

#include <stdlib.h>
#include <stdio.h>

#include "progbase_info.h"

#ifdef __cplusplus
extern "C"{
#endif

/**
 *  @brief reads all input from console into allocated buffer
 *  you should free string after usage with freeString function
 *  @returns a pointer to allocated string buffer
 */
char * getNewString(void);

char getChar(void);
int getInt(void);
long getLong(void);
float getFloat(void);
double getDouble(void);
char * getStringAr(char buffer[], size_t bufferLength);

void printChar(char character);
void printInt(int integer);
void printLong(long longInteger);
void printFloat(float floatingPoint);
void printDouble(double doublePrecision);
void printString(const char buffer[]);

void sleepMillis(unsigned long int milliseconds);

#ifdef __cplusplus
}
#endif
