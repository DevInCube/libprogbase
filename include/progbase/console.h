/** @file
    @brief Console I\O module
*/

#pragma once

#include <stdio.h>

#ifdef __cplusplus
extern "C"{
#endif

#define CON_MIN_ROWS 5
#define CON_MIN_COLS 29

#define INTENSITY 60

enum conAttribute_e {
	ATTR_RESET = 0,
	ATTR_BRIGHT,
	ATTR_DIM,
	ATTR_ITALIC,
	ATTR_UNDERLINE,
	ATTR_REVERSE = 7,
	ATTR_HIDDEN,
	ATTR_CROSSEDOUT,
	FG_BLACK = 30,
	FG_RED,
	FG_GREEN,
	FG_YELLOW,
	FG_BLUE,
	FG_MAGENTA,
	FG_CYAN,
	FG_WHITE,
	FG_DEFAULT = 39,
	BG_BLACK = 40,
	BG_RED,
	BG_GREEN,
	BG_YELLOW,
	BG_BLUE,
	BG_MAGENTA,
	BG_CYAN,
	BG_WHITE,
	BG_DEFAULT = 49,
	FG_INTENSITY_BLACK = 90,
	FG_INTENSITY_RED,
	FG_INTENSITY_GREEN,
	FG_INTENSITY_YELLOW,
	FG_INTENSITY_BLUE,
	FG_INTENSITY_MAGENTA,
	FG_INTENSITY_CYAN,
	FG_INTENSITY_WHITE,
	BG_INTENSITY_BLACK = 100,
	BG_INTENSITY_RED,
	BG_INTENSITY_GREEN,
	BG_INTENSITY_YELLOW,
	BG_INTENSITY_BLUE,
	BG_INTENSITY_MAGENTA,
	BG_INTENSITY_CYAN,
	BG_INTENSITY_WHITE
};

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

struct conpos {
	unsigned int column;
	unsigned int row;
};

struct conpos conGetPos(void);
void conMove(int row, int col);
void conSetAttr(int attr);

void strConMove(char * str, int row, int col);
void strConSetAttr(char * str, int attr);

void conClear(void);
void conReset(void);

int conIsKeyDown(void);
char conGetChar(void);

struct consize {
	unsigned short cols;
	unsigned short rows;
};

struct consize conGetSize(void);
void conResize(unsigned short rows, unsigned short cols);

void conHideCursor(void);
void conShowCursor(void);

void conLockInput(void);
void conUnlockInput(void);

// New Console module interface

typedef struct ConsoleCursorPosition ConsoleCursorPosition;
struct ConsoleCursorPosition {
	unsigned short column;
	unsigned short row;
};

ConsoleCursorPosition Console_cursorPosition(void);
void Console_setCursorPosition(unsigned short row, unsigned short column);
void Console_setCursorAttribute(int attributes);
void Console_setCursorAttributes(int attributes);

void Console_clear(void);
void Console_reset(void);

int Console_isKeyDown(void);
char Console_getChar(void);

typedef struct ConsoleSize ConsoleSize;
struct ConsoleSize {
	unsigned short columns;
	unsigned short rows;
};

ConsoleSize Console_size(void);
void Console_setSize(unsigned short rows, unsigned short columns);

void Console_hideCursor(void);
void Console_showCursor(void);

void Console_lockInput(void);
void Console_unlockInput(void);

#ifdef __cplusplus
}
#endif
