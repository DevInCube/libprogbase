#include <pbconsole.h>
#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#ifndef CON_MIN_ROWS
#	define CON_MIN_ROWS 5
#endif
#ifndef CON_MIN_COLS
#	define CON_MIN_COLS 29
#endif

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
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
	old.c_lflag &= ~ICANON;
	old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror ("tcsetattr ~ICANON");
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
    ioctl(0, TIOCGWINSZ, &w);
	struct consize size;
	size.cols = w.ws_col;
	size.rows = w.ws_row;
	return size;
}
