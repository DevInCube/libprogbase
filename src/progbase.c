#define _POSIX_C_SOURCE 200809L

#include <time.h>

#include "progbase.h"

static char * inputString(FILE* fp, size_t size);

int getInt(void) {
	char input[100];
	fgets(input, 100, stdin);
	return atoi(input);
}

long getLong(void) {
	char input[100];
	fgets(input, 100, stdin);
	return atol(input);
}

float getFloat(void) {
	char input[100];
	fgets(input, 100, stdin);
	return atof(input);
}

double getDouble(void) {
	char input[100];
	fgets(input, 100, stdin);
	return atof(input);
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
    str = realloc(NULL, sizeof(char) * size); /* size is start size */
    if (NULL == str) {
		return str;
	}
    while (EOF != (ch = fgetc(fp)) && ch != '\n') {
        str[len++] = ch;
        if (len == size) {
            str = realloc(str, sizeof(char) * (size += 16));
            if(NULL == str) {
				return str;
			}
        }
    }
    str[len++] = '\0';

    return realloc(str, sizeof(char) * len);
}

void sleepMillis(unsigned long int milliseconds) {
	struct timespec tim;
	tim.tv_sec = 0;
	tim.tv_nsec = milliseconds * 1000000L;
	
	nanosleep(&tim, NULL);
}
