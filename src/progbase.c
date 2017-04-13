#define _POSIX_C_SOURCE 200809L

#include <time.h>

#include <progbase.h>

static char * inputString(FILE* fp, size_t size);

char getChar(void) {
	char val = 0;
	string input = getString();
	val = input[0];
	freeString(input);
	return val;
}

int getInt(void) {
	int val = 0;
	string input = getString();
	val = atoi(input);
	freeString(input);
	return val;
}

long getLong(void) {
	long val = 0L;
	string input = getString();
	val = atol(input);
	freeString(input);
	return val;
}

float getFloat(void) {
	float val = 0;
	string input = getString();
	val = atof(input);
	freeString(input);
	return val;
}

double getDouble(void) {
	double val = 0;
	string input = getString();
	val = atof(input);
	freeString(input);
	return val;
}

string getString(void) {
	return inputString(stdin, 10);
}

void freeString(string str) {
	free(str);
}

char * inputString(FILE * fp, size_t size) {
    /* The size is extended by the input with the value of the provisional */
    char * str;
    int ch;
    size_t len = 0;
    str = (char *)realloc(NULL, sizeof(char) * size);
    if (NULL == str) {
		return str;
	}
    while (EOF != (ch = fgetc(fp)) && ch != '\n') {
        str[len++] = ch;
        if (len == size) {
            str = (char *)realloc(str, sizeof(char) * (size += 16));
            if(NULL == str) {
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
