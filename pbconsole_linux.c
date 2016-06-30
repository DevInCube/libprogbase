#include "pbconsole.h"
#include "stdlib.h"


void conMove(int row, int col) {
	printf("\e[%i;%iH", row, col);
}

void conSetAttr(int attr) {
	printf("\e[%im", attr);
}

void conClear(void) {
	system("clear");
}

void conReset(void) {
	conSetAttr(ATTR_RESET);
}

