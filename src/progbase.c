#define _POSIX_C_SOURCE 200809L

#include <time.h>
#include <string.h>

#include <progbase.h>

static char * inputString(FILE* fp, size_t size);

char getChar(void) {
	char val = 0;
	char * input = getNewString();
	val = input[0];
	free(input);
	return val;
}

int getInt(void) {
	int val = 0;
	char * input = getNewString();
	val = atoi(input);
	free(input);
	return val;
}

long getLong(void) {
	long val = 0L;
	char * input = getNewString();
	val = atol(input);
	free(input);
	return val;
}

float getFloat(void) {
	float val = 0;
	char * input = getNewString();
	val = atof(input);
	free(input);
	return val;
}

double getDouble(void) {
	double val = 0;
	char * input = getNewString();
	val = atof(input);
	free(input);
	return val;
}

char * getNewString(void) {
	return inputString(stdin, 10);
}

char * getNewStringAr(char buffer[], size_t bufferLength) {
	fgets(buffer, bufferLength, stdin);
	buffer[strlen(buffer) - 1] = '\0';
	return buffer;
}

void printChar(char character) { 
	printf("%c", character); 
}

void printInt(int integer) { 
	printf("%d", integer); 
}

void printLong(long longInteger) { 
	printf("%ld", longInteger); 
}

void printFloat(float floatingPoint) {
	printf("%f", floatingPoint);
}

void printDouble(double doublePrecision) {
	printf("%lf", doublePrecision);
}

void printString(const char buffer[]) {
	printf("%s", buffer);
}

static char * inputString(FILE * fp, size_t size) {
    /* The size is extended by the input with the value of the provisional */
    char * str;
    char * strold;
    int ch;
    size_t len = 0;
    str = (char *)realloc(NULL, sizeof(char) * size);
    if (NULL == str) {
		return str;
	}
    while (EOF != (ch = fgetc(fp)) && ch != '\n') {
        str[len++] = ch;
        if (len == size) {
            strold = str;
            str = (char *)realloc(str, sizeof(char) * (size += 16));
            if(NULL == str) {
                free(strold);
				return str;
			}
        }
    }
    str[len++] = '\0';

    return (char *)realloc(str, sizeof(char) * len);
}

void sleepMillis(unsigned long int milliseconds) {
	struct timespec tim;
	tim.tv_sec = (milliseconds / 1000L);
	tim.tv_nsec = (milliseconds % 1000L) * 1000000L;

	nanosleep(&tim, NULL);
}
