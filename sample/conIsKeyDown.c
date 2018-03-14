#include <stdio.h>

#include <progbase.h>
#include <progbase/console.h>

int main(void) {
	printf("Press any key to stop this!\n");
	while(!Console_isKeyDown()) {
		putchar('*');
		sleepMillis(50);
	}
	printf("\nYou've pressed key '%c'\n", Console_getChar());
	return 0;
}
