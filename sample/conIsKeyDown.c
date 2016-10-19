#include <progbase.h>
#include <pbconsole.h>
#include <stdio.h>

int main(void) {
	printf("Press any key to stop this!\n");
	while(!conIsKeyDown()) {
		putchar('*');
		sleepMillis(50);
	}
	printf("\nYou've pressed key '%c'\n", getchar());
	return 0;
}
