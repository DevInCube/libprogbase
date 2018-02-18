#define _WIN_SOURCE

#include <Windows.h>
#include <string.h>
#include <conio.h>

#include <progbase\console.h>

#ifndef CON_MIN_ROWS
#	define CON_MIN_ROWS 5
#endif
#ifndef CON_MIN_COLS
#	define CON_MIN_COLS 29
#endif

enum WinConsoleColor {
	black = 0,
	blue = 1,
	green = 2,
	cyan = 3,
	red = 4,
	magenta = 5,
	brown = 6,
	lightgray = 7,
	darkgray = 8,
	lightblue = 9,
	lightgreen = 10,
	lightcyan = 11,
	lightred = 12,
	lightmagenta = 13,
	yellow = 14,
	white = 15
};

static int FG_COLOR_CHOOSER(int FG) {
	if(FG == FG_BLACK)
		return darkgray;
	else if(FG == FG_RED)
		return red;
	else if(FG == FG_GREEN)
		return green;
	else if(FG == FG_YELLOW)
		return yellow;
	else if(FG == FG_BLUE)
		return blue;
	else if(FG == FG_MAGENTA)
		return magenta;
	else if(FG == FG_CYAN)
		return cyan;
	else if(FG == FG_WHITE)
		return lightgray;
	else if(FG == FG_DEFAULT)
		return 15;
	else if(FG == FG_INTENSITY_BLACK)
		return black;
	else if(FG == FG_INTENSITY_RED)
		return lightred;
	else if(FG == FG_INTENSITY_GREEN)
		return lightgreen;
	else if(FG == FG_INTENSITY_YELLOW)
		return yellow;
	else if(FG == FG_INTENSITY_BLUE)
		return lightblue;
	else if(FG == FG_INTENSITY_MAGENTA)
		return lightmagenta;
	else if(FG == FG_INTENSITY_CYAN)
		return lightcyan;
	else if(FG == FG_INTENSITY_WHITE)
		return white;
}

static int BG_COLOR_CHOOSER(int BG) {
	if(BG == BG_BLACK)
		return darkgray;
	else if(BG == BG_RED)
		return red;
	else if(BG == BG_GREEN)
		return green;
	else if(BG == BG_YELLOW)
		return yellow;
	else if(BG == BG_BLUE)
		return blue;
	else if(BG == BG_MAGENTA)
		return magenta;
	else if(BG == BG_CYAN)
		return cyan;
	else if(BG == BG_WHITE)
		return lightgray;
	else if(BG == BG_DEFAULT)
		return 0;
	else if(BG == BG_INTENSITY_BLACK)
		return black;
	else if(BG == BG_INTENSITY_RED)
		return lightred;
	else if(BG == BG_INTENSITY_GREEN)
		return lightgreen;
	else if(BG == BG_INTENSITY_YELLOW)
		return yellow;
	else if(BG == BG_INTENSITY_BLUE)
		return lightblue;
	else if(BG == BG_INTENSITY_MAGENTA)
		return lightmagenta;
	else if(BG == BG_INTENSITY_CYAN)
		return lightcyan;
	else if(BG == BG_INTENSITY_WHITE)
		return white;
}

static char WinConsoleColorToColorSchemeColor(const int color) {
		if(color <= lightblue)
			return '0' + color;
		else if(color > lightgreen)
			return 'A' + color - 10;
}

void Console_winMainColorScheme(const int background, const int text) {
	char back = WinConsoleColorToColorSchemeColor(background);
	char txt = WinConsoleColorToColorSchemeColor(text);
	char setColor[8];
	sprintf(setColor, "color %c%c", back, txt);
	system(setColor);
}

void Console_winSetColorAttributes(const int background, const int text) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (WORD) ((background << 4) | text));
}

struct conpos conGetPos(void) {
	struct conpos pos;
	CONSOLE_SCREEN_BUFFER_INFO SBInfo;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &SBInfo);
	pos.row = SBInfo.dwCursorPosition.X;
	pos.column = SBInfo.dwCursorPosition.Y;
	return pos;
}

void conMove(int row, int col) {
	COORD cursorPos;
	cursorPos.X = col;
  	cursorPos.Y = row;
  	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPos);
}

void conSetAttr(int attr) {
 	WORD setColor;
 	HANDLE consoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
 	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(consoleOutput, &csbi);

	if((attr >= FG_BLACK && attr <= FG_DEFAULT) ||
	 (attr >= FG_INTENSITY_BLACK && attr <= FG_INTENSITY_WHITE))
		setColor = (csbi.wAttributes & 0xF0) + (FG_COLOR_CHOOSER(attr) & 0x0F);
	else if((attr >= BG_BLACK && attr <= BG_DEFAULT) ||
	 (attr >= BG_INTENSITY_BLACK && attr <= BG_INTENSITY_WHITE))
		setColor = (BG_COLOR_CHOOSER(attr) << 4 & 0xF0) | (csbi.wAttributes & 0x0F);

	SetConsoleTextAttribute(consoleOutput, setColor);
}

void strConMove(char * str, int row, int col) {
}

void strConSetAttr(char * str, int attr) {
}

void conClear(void) {
	system("cls");
}

void conReset(void) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), black | white);
}

int conIsKeyDown(void) {
	return kbhit();
}

char conGetChar(void) {
	return getch();
}

struct consize conGetSize(void) {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	return (struct consize) {
		.cols = csbi.srWindow.Right - csbi.srWindow.Left + 1,
		.rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1
	};
}

void conResize(unsigned short rows, unsigned short cols) {
	HWND console = GetConsoleWindow();
	RECT r;
	GetWindowRect(console, &r);
	MoveWindow(console, r.left, r.top, rows, cols, TRUE);
}


void conHideCursor(void) {
   CONSOLE_CURSOR_INFO info;
   info.dwSize = 100;
   info.bVisible = FALSE;
   SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
}

void conShowCursor(void) {
   CONSOLE_CURSOR_INFO info;
   info.dwSize = 100;
   info.bVisible = TRUE;
   SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
}

void conLockInput(void) {
    DWORD mode = 0;
    GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &mode);
	SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), mode & (~ENABLE_ECHO_INPUT));
}

void conUnlockInput(void) {
	DWORD mode = 0;
    GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &mode);
	SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), mode | ENABLE_ECHO_INPUT);
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

void Console_setCursorAttribute(int attributes) {
	conSetAttr(attributes);
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
