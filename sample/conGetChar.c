#include <stdio.h>

#include <progbase.h>
#include <progbase/console.h>

int main(void) {
	char key = 0;
	int box_left = 10;
	int box_top = 3;
	int box_width = 60;
	int box_height = 18;
	int i = 0;
	int x = 40;
	int y = 5;
	int clear = 1;
	struct ConsoleCursorPosition pos;
	atexit(Console_clear);
	Console_clear();
	printf("Use 'wasd' to move, 'c' to toggle clear, 'q' to quit:\n");
	for (i = box_left; i <= box_left + box_width; i++) {
		Console_setCursorPosition(box_top, i);
		putchar('=');
		Console_setCursorPosition(box_top + box_height, i);
		putchar('=');
	}
	for (i = box_top + 1; i < box_top + box_height; i++) {
		Console_setCursorPosition(i, box_left);
		putchar('|');
		Console_setCursorPosition(i, box_left + box_width);
		putchar('|');
	}
	Console_setCursorPosition(y, x);
	putchar('@');
	Console_hideCursor();
	while (key != 'q') {
		Console_setCursorPosition(2, 1);
		key = Console_getChar();
		if (clear) {
			Console_setCursorPosition(y, x);
			putchar(' ');
		}
		switch (key) {
		case 'w':
			y--;
			break;
		case 'a':
			x--;
			break;
		case 's':
			y++;
			break;
		case 'd':
			x++;
			break;
		case 'c':
			clear = !clear;
			break;
		}
		if (x <= box_left) x = box_left + 1;
		if (x >= box_left + box_width) x = box_left + box_width - 1;
		if (y <= box_top) y = box_top + 1;
		if (y >= box_top + box_height) y = box_top + box_height - 1;
		Console_setCursorPosition(y, x);
		pos = Console_cursorPosition();
		putchar('@');
		Console_setCursorPosition(3, 1);
		printf("Row: %i\nCol: %i", pos.row, pos.column);
	}
	printf("\nThe end.\n");
	Console_showCursor();
	return 0;
}
