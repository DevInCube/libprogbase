#include <stdio.h>

#include <progbase.h>
#include <progbase/console.h>

int main(void) {
	int i = 0;
	string str = NULL;
	printf("Please, enter something: ");
	str = getString();
	puts(str);
	conClear();
	for (i = 0; i < 110; i++) {
		Console_reset();
		Console_setCursorAttribute(i);
		printf(" [%i] ", i);
	}
	puts("");
	Console_reset();
	return 0;
}
