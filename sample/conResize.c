#include <stdio.h>

#include <progbase.h>
#include <progbase/console.h>

int main(void) {
	char key = 0;
	int resize = 0;
	unsigned short width = 40;
	unsigned short height = 20;
	struct ConsoleSize size;
	Console_clear();
	Console_setSize(height, width);
	while (key != 'q') {
		Console_clear();
		printf("Use 'wasd' to change terminal size,\n'q' to quit:\n");
		size = Console_size();
		printf("rows: %i\ncols: %i", size.rows, size.columns);
		key = Console_getChar();
		resize = 1;
		switch (key) {
		case 'w':
			height--;
			break;
		case 'a':
			width--;
			break;
		case 's':
			height++;
			break;
		case 'd':
			width++;
			break;
		default:
			resize = 0;
		}
		if (width < CON_MIN_COLS) width = CON_MIN_COLS;
		if (height < CON_MIN_ROWS) height = CON_MIN_ROWS;
		if (resize) {
			Console_setSize(height, width);
		}
	}
	printf("\nThe end.\n");
	return 0;
}
