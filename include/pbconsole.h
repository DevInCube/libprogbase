#ifndef PBCONSOLE_H
#define PBCONSOLE_H

#include <stdio.h>

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

struct conpos {
	unsigned int column;
	unsigned int row;
};

#ifdef __cplusplus
extern "C"{
#endif

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

#ifdef __cplusplus
}
#endif

#endif
