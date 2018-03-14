#include <stdio.h>

#include <progbase.h>
#include <progbase/console.h>

int main(void) {
	char key = 0;
	int resize = 0;
	unsigned short width = 40;
	unsigned short height = 20;
	struct consize size;
	conClear();
	conResize(height, width);
	while (key != 'q') {
		conClear();
		printf("Use 'wasd' to change terminal size,\n'q' to quit:\n");
		size = conGetSize();
		printf("rows: %i\ncols: %i", size.rows, size.cols);
		key = conGetChar();
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
			conResize(height, width);
		}
	}
	printf("\nThe end.\n");
	return 0;
}
