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
	WIN_BLACK, // 0
	WIN_BLUE,
	WIN_GREEN,
	WIN_CYAN,
	WIN_RED,
	WIN_MAGENTA,
	WIN_BROWN,
	WIN_LIGHTGRAY,
	WIN_DARKGRAY,
	WIN_LIGHTBLUE,
	WIN_LIGHTGREEN,
	WIN_LIGHTCYAN,
	WIN_LIGHTRED,
	WIN_LIGHTMAGENTA,
	WIN_YELLOW,
	WIN_WHITE // 15
};

static int fgColorChooser(const int fontGround) {
	if(fontGround == FG_BLACK)
		return WIN_BLACK;
	else if(fontGround == FG_RED)
		return WIN_RED;
	else if(fontGround == FG_GREEN)
		return WIN_GREEN;
	else if(fontGround == FG_YELLOW)
		return WIN_BROWN;
	else if(fontGround == FG_BLUE)
		return WIN_BLUE;
	else if(fontGround == FG_MAGENTA)
		return WIN_MAGENTA;
	else if(fontGround == FG_CYAN)
		return WIN_CYAN;
	else if(fontGround == FG_WHITE)
		return WIN_LIGHTGRAY;
	else if(fontGround == FG_DEFAULT)
		return WIN_WHITE;
	else if(fontGround == FG_INTENSITY_BLACK)
		return WIN_DARKGRAY;
	else if(fontGround == FG_INTENSITY_RED)
		return WIN_LIGHTRED;
	else if(fontGround == FG_INTENSITY_GREEN)
		return WIN_LIGHTGREEN;
	else if(fontGround == FG_INTENSITY_YELLOW)
		return WIN_YELLOW;
	else if(fontGround == FG_INTENSITY_BLUE)
		return WIN_LIGHTBLUE;
	else if(fontGround == FG_INTENSITY_MAGENTA)
		return WIN_LIGHTMAGENTA;
	else if(fontGround == FG_INTENSITY_CYAN)
		return WIN_LIGHTCYAN;
	else if(fontGround == FG_INTENSITY_WHITE)
		return WIN_WHITE;
	return WIN_WHITE;  // default
}

static int bgColorChooser(const int backGround) {
	if(backGround == BG_BLACK)
		return WIN_BLACK;
	else if(backGround == BG_RED)
		return WIN_RED;
	else if(backGround == BG_GREEN)
		return WIN_GREEN;
	else if(backGround == BG_YELLOW)
		return WIN_BROWN;
	else if(backGround == BG_BLUE)
		return WIN_BLUE;
	else if(backGround == BG_MAGENTA)
		return WIN_MAGENTA;
	else if(backGround == BG_CYAN)
		return WIN_CYAN;
	else if(backGround == BG_WHITE)
		return WIN_LIGHTGRAY;
	else if(backGround == BG_DEFAULT)
		return WIN_BLACK;
	else if(backGround == BG_INTENSITY_BLACK)
		return WIN_DARKGRAY;
	else if(backGround == BG_INTENSITY_RED)
		return WIN_LIGHTRED;
	else if(backGround == BG_INTENSITY_GREEN)
		return WIN_LIGHTGREEN;
	else if(backGround == BG_INTENSITY_YELLOW)
		return WIN_YELLOW;
	else if(backGround == BG_INTENSITY_BLUE)
		return WIN_LIGHTBLUE;
	else if(backGround == BG_INTENSITY_MAGENTA)
		return WIN_LIGHTMAGENTA;
	else if(backGround == BG_INTENSITY_CYAN)
		return WIN_LIGHTCYAN;
	else if(backGround == BG_INTENSITY_WHITE)
		return WIN_WHITE;
	return WIN_BLACK;  // default
}

ConsoleCursorPosition Console_cursorPosition(void) {
	ConsoleCursorPosition pos;
	CONSOLE_SCREEN_BUFFER_INFO SBInfo;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &SBInfo);
	pos.row = SBInfo.dwCursorPosition.X + 1;
	pos.column = SBInfo.dwCursorPosition.Y + 1;
	return pos;
}

void Console_setCursorPosition(unsigned short row, unsigned short column) {
	COORD cursorPos;
	cursorPos.X = column - 1;
  	cursorPos.Y = row - 1;
  	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPos);
}

void Console_setCursorAttribute(int attributes) {
 	WORD setColor;
 	HANDLE consoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
 	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(consoleOutput, &csbi);

	if((attributes >= FG_BLACK && attributes <= FG_DEFAULT) ||
	 (attributes >= FG_INTENSITY_BLACK && attributes <= FG_INTENSITY_WHITE))
		setColor = (csbi.wAttributes & 0xF0) + (fgColorChooser(attributes) & 0x0F);
	else if((attributes >= BG_BLACK && attributes <= BG_DEFAULT) ||
	 (attributes >= BG_INTENSITY_BLACK && attributes <= BG_INTENSITY_WHITE))
		setColor = (bgColorChooser(attributes) << 4 & 0xF0) | (csbi.wAttributes & 0x0F);

	SetConsoleTextAttribute(consoleOutput, setColor);
}

void Console_setCursorAttributes(int attributes) {
	Console_setCursorAttribute(attributes);
}

void Console_clear(void) {
	system("cls");
}

void Console_reset(void) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WIN_BLACK | WIN_WHITE);
}

int Console_isKeyDown(void) {
	return kbhit();
}

char Console_getChar(void) {
	return getch();
}

ConsoleSize Console_size(void) {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	return (ConsoleSize) {
		.columns = csbi.srWindow.Right - csbi.srWindow.Left + 1,
		.rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1
	};
}

void Console_setSize(unsigned short rows, unsigned short columns) {
	if (columns < CON_MIN_COLS) columns = CON_MIN_COLS;
	if (columns > 999) columns = 999;
	if (rows < CON_MIN_ROWS) rows = CON_MIN_ROWS;
	if (rows > 999) rows = 999;
	HWND console = GetConsoleWindow();
	RECT r;
	// formula which calculate rows/columns into pixel size
	rows = (short)(((double) rows * 15 + /*size of window manager panel*/ 69));
	columns = (short)(((double) columns * 7.3 + /*size of scrolling panel*/ 39));
	/* TODO */
	// find other, not by-pixel realization
	GetWindowRect(console, &r);
	MoveWindow(console, r.left, r.top, columns, rows, TRUE);
}

void Console_hideCursor(void) {
   CONSOLE_CURSOR_INFO info;
   info.dwSize = 100;
   info.bVisible = FALSE;
   SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
}

void Console_showCursor(void) {
   CONSOLE_CURSOR_INFO info;
   info.dwSize = 100;
   info.bVisible = TRUE;
   SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
}

void Console_lockInput(void) {
    DWORD mode = 0;
    GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &mode);
	SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), mode & (~ENABLE_ECHO_INPUT));
}

void Console_unlockInput(void) {
	DWORD mode = 0;
    GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &mode);
	SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), mode | ENABLE_ECHO_INPUT);
}

// fix the problem in all Windows distributions
// which prevence fullscreen command prompt window
/*
void Console_winFullScreenFix(void) {
	system("mode 800");
}
*/


// old interface implementations
// lines 224 - 290, if neeed to be deleted

struct conpos conGetPos(void) {
	ConsoleCursorPosition pos = Console_cursorPosition();
	return (struct conpos) {
		.row = pos.row,
		.column = pos.column
	};
}

void conMove(int row, int col) {
	Console_setCursorPosition(row,col);
}

void conSetAttr(int attr) {
	Console_setCursorAttribute(attr);
}

void strConMove(char * str, int row, int col) {
	/* TODO */
}

void strConSetAttr(char * str, int attr) {
	/* TODO */
}

void conClear(void) {
	Console_clear();
}

void conReset(void) {
	Console_reset();
}

int conIsKeyDown(void) {
	return Console_isKeyDown();
}

char conGetChar(void) {
	return Console_getChar();
}

struct consize conGetSize(void) {
	ConsoleSize size = Console_size();
	return (struct consize) {
		.cols = size.columns,
		.rows = size.rows
	};
}

void conResize(unsigned short rows, unsigned short cols) {
	Console_setSize(rows,cols);
}

void conHideCursor(void) {
	Console_hideCursor();
}

void conShowCursor(void) {
	Console_showCursor();
}

void conLockInput(void) {
	Console_lockInput();
}

void conUnlockInput(void) {
	Console_unlockInput();
}
