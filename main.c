#include <stdio.h>
#include "pbconsole.h"

int main(void) {
	int i = 0;
	conClear();
	for (i = 0; i < 110; i++) {
		conReset();
		conSetAttr(i);
		printf(" [%i] ", i);
	}
	conReset();
	return 0;
}
