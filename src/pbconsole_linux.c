#define _DEFAULT_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <progbase/console.h>

#ifndef CON_MIN_ROWS
#	define CON_MIN_ROWS 5
#endif
#ifndef CON_MIN_COLS
#	define CON_MIN_COLS 29
#endif

void conMove(int row, int col) {
	printf("\033[%i;%iH", row, col);
	fflush(stdout);
}

void conSetAttr(int attr) {
	printf("\033[%im", attr);
	fflush(stdout);
}

void strConMove(char * str, int row, int col) {
	char tmp[20] = "";
	sprintf(tmp, "\033[%i;%iH", row, col);
	strcat(str, tmp);
}

void strConSetAttr(char * str, int attr) {
	char tmp[20] = "";
	sprintf(tmp, "\033[%im", attr);
	strcat(str, tmp);
}

void conHideCursor(void) {
	printf("\033[?25l");
	fflush(stdout);
}

void conShowCursor(void) {
	printf("\033[?25h");
	fflush(stdout);
}

void conClear(void) {
	system("clear");
}

void conReset(void) {
	conSetAttr(ATTR_RESET);
}

int conIsKeyDown(void) {
	struct termios oldt, newt;
	int ch;
	int oldf;

	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

	ch = getchar();

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);

	if(ch != EOF) {
		ungetc(ch, stdin);
		return 1;
	}
	return 0;
}

char conGetChar(void) {
    char buf = 0;
    struct termios old = {0};
    fflush(stdout);
	old.c_lflag &= ~ECHO;
    if (tcgetattr(0, &old) < 0) {
        perror("tcsetattr()");
	}
	old.c_lflag &= ~ICANON;
	old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0) {
        perror("tcsetattr ICANON");
	}
    if (read(0, &buf, 1) < 0) {
        perror("read()");
	}
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0) {
        perror ("tcsetattr ~ICANON");
	}
    return buf;
}

void conResize(unsigned short rows, unsigned short cols) {
	char buf[35] = "";
	if (cols < CON_MIN_COLS) cols = CON_MIN_COLS;
	if (cols > 999) cols = 999;
	if (rows < CON_MIN_ROWS) rows = CON_MIN_ROWS;
	if (rows > 999) rows = 999;
	sprintf(buf, "resize -s %i %i > /dev/null", rows, cols);
	system(buf);
}

struct consize conGetSize(void) {
    struct winsize w;
	struct consize size;
    ioctl(0, TIOCGWINSZ, &w);
	size.cols = w.ws_col;
	size.rows = w.ws_row;
	return size;
}

struct conpos conGetPos(void) {
	struct conpos pos = {0, 0};
	char buf[8];
	char cmd[] = "\033[6n";
	struct termios save, raw;
	fflush(stdout);
	tcgetattr(0, &save);
	cfmakeraw(&raw);
	tcsetattr(0, TCSANOW, &raw);
	if (isatty(fileno(stdin))) {
		write(1, cmd, sizeof(cmd));
		read (0, buf ,sizeof(buf));
		sscanf(buf, "\033[%u;%u", &(pos.row), &(pos.column));
	}
	tcsetattr(0, TCSANOW, &save);
	fflush(stdout);
	return pos;
}

static struct termios orig;

void conLockInput(void) {
    struct termios new;
    assert(0 == tcgetattr(STDIN_FILENO, &orig));
    memcpy(&new, &orig, sizeof(struct termios));
    new.c_lflag &= !(ECHO | ICANON | ECHOE | ECHOK | ECHONL | ICRNL);
    new.c_cc[VTIME] = 0;
    new.c_cc[VMIN] = 0;
    assert(0 == tcsetattr(STDIN_FILENO, TCSANOW, &new));
}

void conUnlockInput(void) {
    assert(0 == tcsetattr(STDIN_FILENO, TCSANOW, &orig));
}

// new interface implementation

ConsoleCursorPosition Console_cursorPosition(void) { 
	struct conpos pos = conGetPos();
	return (ConsoleCursorPosition) {
		.column = pos.column,
		.row = pos.row
	};
}

void Console_setCursorPosition(unsigned short row, unsigned short column) {
	conMove(row, column);
}

void Console_setCursorAttributes(int attributes) {
	conSetAttr(attributes);
}

void Console_clear(void) {
	conClear();
}

void Console_reset(void) {
	conReset();
}

int Console_isKeyDown(void) {
	return conIsKeyDown();
}

char Console_getChar(void) {
	return conGetChar();
}

ConsoleSize Console_size(void) {
	struct consize size = conGetSize();
	return (ConsoleSize) {
		.columns = size.cols,
		.rows = size.rows
	};
}

void Console_setSize(unsigned short rows, unsigned short columns) {
	conResize(rows, columns);
}

void Console_hideCursor(void) {
	conHideCursor();
}

void Console_showCursor(void) {
	conShowCursor();
}

void Console_lockInput(void) {
	conLockInput();
}

void Console_unlockInput(void) {
	conUnlockInput();
}
