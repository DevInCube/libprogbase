#include <stdio.h>
#include <stdlib.h>
#include "console.h"
#include "canvas.h"

typedef unsigned char CanvasColor;

CanvasColor *canvas = NULL;
CanvasColor currentColor = 0;

int originRow = 1;
int originColumn = 1;

int width = 0;
int height = 0;

char startedDrawing = 0;
char yOrientation = 0; // 0 = top->down, 1 = down->top

CanvasColor black = 0;

#define CANVAS_AT(X, Y) canvas[(X) + (Y)*width]
#define IN_BOUNDS(X, A, B) ((X) >= (A) && (X) <= (B))
#define CHECK_DRAWING(X)       \
    if (startedDrawing != (X)) \
        return;
#define ABS(X) (((X) < 0) ? -(X) : (X))
#define SET_COL_BG "\033[48;5;%im"
#define SET_COL_FG "\033[38;5;%im"

void Canvas_setOrigin(int conRow, int conColumn)
{
    CHECK_DRAWING(0);

    if (conRow >= 1)
        originRow = conRow;

    if (conColumn >= 1)
        originColumn = conColumn;
}

void Canvas_setSize(int widthPixels, int heightPixels)
{
    CHECK_DRAWING(0);

    if (widthPixels < 0 || heightPixels < 0)
    {
        return;
    }

    if (canvas)
    {
        free(canvas);
    }

    canvas = (CanvasColor *)malloc(sizeof(CanvasColor) * widthPixels * heightPixels);

    if (canvas != NULL)
    {
        width = widthPixels;
        height = heightPixels;
    }
    else
    {
        width = 0;
        height = 0;
    }
}

void Canvas_invertYOrientation(void)
{
    CHECK_DRAWING(0);

    yOrientation = 1 - yOrientation;
}

static CanvasColor EncodeColor(unsigned char r, unsigned char g, unsigned char b)
{
    r /= 51;
    g /= 51;
    b /= 51;

    return 16 + 36 * r + 6 * g + b;
}

void Canvas_setColorRGB(int red, int green, int blue)
{
    CHECK_DRAWING(1);

    if (IN_BOUNDS(red, 0, 255) && IN_BOUNDS(green, 0, 255) && IN_BOUNDS(blue, 0, 255))
    {
        currentColor = EncodeColor(red, green, blue);
    }
}

void Canvas_setColorValue(int value) 
{
    CHECK_DRAWING(1);

    if (IN_BOUNDS(value, 0, 255))
    {
        currentColor = value;
    }
}

void Canvas_beginDraw(void)
{
    CHECK_DRAWING(0);

    Console_hideCursor();

    for (int i = 0; i < width * height; i++)
    {
        canvas[i] = black;
    }
    currentColor = black;

    startedDrawing = 1;
}

void Canvas_putPixel(int x, int y)
{
    CHECK_DRAWING(1);

    if (IN_BOUNDS(x, 0, width - 1) && IN_BOUNDS(y, 0, height - 1))
    {
        CANVAS_AT(x, y) = currentColor;
    }
}

void Canvas_strokeLine(int x0, int y0, int x1, int y1)
{
    CHECK_DRAWING(1);

    int dx = ABS(x1 - x0);
    int dy = ABS(y1 - y0);
    int sx = x1 >= x0 ? 1 : -1;
    int sy = y1 >= y0 ? 1 : -1;

    Canvas_putPixel(x0, y0);

    if (dy <= dx)
    {
        int d = (dy << 1) - dx;
        int d1 = dy << 1;
        int d2 = (dy - dx) << 1;
        for (int x = x0 + sx, y = y0, i = 1; i <= dx; i++, x += sx)
        {
            if (d > 0)
            {
                d += d2;
                y += sy;
            }
            else
            {
                d += d1;
            }
            Canvas_putPixel(x, y);
        }
    }
    else
    {
        int d = (dx << 1) - dy;
        int d1 = dx << 1;
        int d2 = (dx - dy) << 1;
        for (int y = y0 + sy, x = x0, i = 1; i <= dy; i++, y += sy)
        {
            if (d > 0)
            {
                d += d2;
                x += sx;
            }
            else
            {
                d += d1;
            }
            Canvas_putPixel(x, y);
        }
    }
}

void Canvas_strokeRect(int x, int y, int width, int height)
{
    for (int i = y; i < y + height; i++)
    {
        Canvas_putPixel(x, i);
        Canvas_putPixel(x + width - 1, i);
    }
    for (int j = x; j < x + width; j++)
    {
        Canvas_putPixel(j, y);
        Canvas_putPixel(j, y + height - 1);
    }
}

void Canvas_fillRect(int x, int y, int width, int height)
{
    for (int i = y; i < y + height; i++)
    {
        for (int j = x; j < x + width; j++)
        {
            Canvas_putPixel(j, i);
        }
    }
}

void Canvas_strokeCircle(int x0, int y0, int radius)
{
    CHECK_DRAWING(1);
    if (radius <= 0)
        return;

    int x = radius - 1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);

    while (x >= y)
    {
        Canvas_putPixel(x0 + x, y0 + y);
        Canvas_putPixel(x0 + y, y0 + x);
        Canvas_putPixel(x0 - y, y0 + x);
        Canvas_putPixel(x0 - x, y0 + y);
        Canvas_putPixel(x0 - x, y0 - y);
        Canvas_putPixel(x0 - y, y0 - x);
        Canvas_putPixel(x0 + y, y0 - x);
        Canvas_putPixel(x0 + x, y0 - y);

        if (err <= 0)
        {
            y++;
            err += dy;
            dy += 2;
        }

        if (err > 0)
        {
            x--;
            dx += 2;
            err += dx - (radius << 1);
        }
    }
}

void Canvas_fillCircle(int x0, int y0, int radius)
{
    CHECK_DRAWING(1);
    if (radius <= 0)
        return;

    for (int y = -radius; y <= radius; y++)
        for (int x = -radius; x <= radius; x++)
            if ((x * x) + (y * y) <= (radius * radius))
                Canvas_putPixel(x0 + x, y0 + y);
}

void Canvas_endDraw(void)
{
    CHECK_DRAWING(1);

    int rCol_UP = -1;
    int rCol_DOWN = -1;

    char setPosManually = originRow || originColumn;

    int y;
    for (int yi = 0; yi < height / 2; yi++)
    {
        y = yi;
        if (yOrientation)
        {
            y = (height / 2) - yi - 1;
        }

        if (setPosManually)
        {
            Console_setCursorPosition(originRow + yi, originColumn);
        }

        for (int x = 0; x < width; x++)
        {
            int newCol_UP = CANVAS_AT(x, y * 2);

            if (newCol_UP != rCol_UP)
            {
                printf(yOrientation ? SET_COL_BG : SET_COL_FG, newCol_UP);
                rCol_UP = newCol_UP;
            }

            int newCol_DOWN = CANVAS_AT(x, y * 2 + 1);

            if (newCol_DOWN != rCol_DOWN)
            {
                printf(yOrientation ? SET_COL_FG : SET_COL_BG, newCol_DOWN);
                rCol_DOWN = newCol_DOWN;
            }

            fwrite("\xE2\x96\x80", 1, 3, stdout);
        }

        if (!setPosManually)
        {
            fwrite("\n", 1, 1, stdout);
        }
    }
    Console_reset();
    Console_showCursor();

    startedDrawing = 0;
}
