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
		conReset();
		conSetAttr(i);
		printf(" [%i] ", i);
	}
	puts("");
	conReset();
	return 0;
}
