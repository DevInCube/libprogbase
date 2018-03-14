#include <stdio.h>

#include <progbase.h>
#include <progbase/console.h>

int main(void) {
	printf("Please, enter something: ");
	char * str = getNewString();
	puts(str);
	Console_clear();
	for (int i = 0; i < 110; i++) {
		Console_reset();
		Console_setCursorAttribute(i);
		printf(" [%i] ", i);
	}
	puts("");
	Console_reset();
	free(str);
	return 0;
}
