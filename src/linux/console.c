#define _DEFAULT_SOURCE
#define _POSIX_SOURCE

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

ConsoleCursorPosition Console_cursorPosition(void) {
	ConsoleCursorPosition pos = {0, 0};
	char cmd[] = "\033[6n";
	struct termios save = {0}, raw = {0};
	fflush(stdout);
	tcgetattr(0, &save);
	cfmakeraw(&raw);
	tcsetattr(0, TCSANOW, &raw);
	if (isatty(fileno(stdin))) {
        char buf[8] = {0};
		write(1, cmd, sizeof(cmd));
		read (0, buf, sizeof(buf));
		sscanf(buf, "\033[%hu;%hu", &(pos.row), &(pos.column));
	}
	tcsetattr(0, TCSANOW, &save);
	fflush(stdout);
	return pos;
}

void Console_setCursorPosition(unsigned short row, unsigned short column) {
	printf("\033[%i;%iH", row, column);
	fflush(stdout);
}

void Console_setCursorAttribute(int attributes) {
	printf("\033[%im", attributes);
	fflush(stdout);
}

void Console_setCursorAttributes(int attributes) {
	Console_setCursorAttribute(attributes);
}

void Console_clear(void) {
	system("clear");
}

void Console_reset(void) {
	Console_setCursorAttribute(ATTR_RESET);
}

int Console_isKeyDown(void) {
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

char Console_getChar(void) {
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

ConsoleSize Console_size(void) {
	struct winsize w = {0};
    ioctl(0, TIOCGWINSZ, &w);
	return (ConsoleSize) {
		.columns = w.ws_col,
		.rows = w.ws_row
	};
}

void Console_setSize(unsigned short rows, unsigned short columns) {
	char buf[35] = "";
	const int CON_MAX_SIZE = 999;
	if (columns < CON_MIN_COLS) columns = CON_MIN_COLS;
	if (columns > CON_MAX_SIZE) columns = CON_MAX_SIZE;
	if (rows < CON_MIN_ROWS) rows = CON_MIN_ROWS;
	if (rows > CON_MAX_SIZE) rows = CON_MAX_SIZE;
	sprintf(buf, "resize -s %i %i > /dev/null", rows, columns);
	system(buf);
}

void Console_hideCursor(void) {
	printf("\033[?25l");
	fflush(stdout);
}

void Console_showCursor(void) {
	printf("\033[?25h");
	fflush(stdout);
}

static struct termios orig;

void Console_lockInput(void) {
	struct termios new;
    assert(0 == tcgetattr(STDIN_FILENO, &orig));
    memcpy(&new, &orig, sizeof(struct termios));
    new.c_lflag &= !(ECHO | ICANON | ECHOE | ECHOK | ECHONL | ICRNL);
    new.c_cc[VTIME] = 0;
    new.c_cc[VMIN] = 0;
    assert(0 == tcsetattr(STDIN_FILENO, TCSANOW, &new));
}

void Console_unlockInput(void) {
	assert(0 == tcsetattr(STDIN_FILENO, TCSANOW, &orig));
}
