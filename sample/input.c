#include <stdio.h>
#include <stdlib.h>
#include <progbase.h>

int main(void) {
	char c = '\0';
    int i = 0;
    long l = 0L;
    float f = 0.0F;
    double d = 0.0F;
    char * s = NULL;

	printf("Enter char: ");
    c = getChar();
    printf("Entered: %c\n", c);

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
    printf("Entered: %f\n", d);

    printf("Enter string: ");
    s = getNewString();
    printf("Entered: %s\n", s);

    free(s);
    return 0;
}
