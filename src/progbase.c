#include "progbase.h"

static char * inputString(FILE* fp, size_t size);

int getInt(void) {
	int integer = 0;
	scanf("%i", &integer);
	return integer;
}

long getLong(void) {
	long lng = 0L;
	scanf("%ld", &lng);
	return lng;
}

float getFLoat(void) {
	float flt = 0.0;
	scanf("%f", &flt);
	return flt;
}

double getDouble(void) {
	double dbl = 0.0;
	scanf("%lf", &dbl);
	return dbl;
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
