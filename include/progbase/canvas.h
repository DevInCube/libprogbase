/** @file
    @brief Console drawing Canvas module
*/

#pragma once 

void Canvas_setOrigin(int conRow, int conColumn);

void Canvas_setSize(int widthPixels, int heightPixels);

void Canvas_invertYOrientation(void);

void Canvas_setColorRGB(int red, int green, int blue);

void Canvas_setColorValue(int value);

void Canvas_beginDraw(void);

void Canvas_putPixel(int x, int y);

void Canvas_strokeRect(int x, int y, int width, int height);

void Canvas_fillRect(int x, int y, int width, int height);

void Canvas_strokeCircle(int x0, int y0, int radius);

void Canvas_fillCircle(int x0, int y0, int radius);

void Canvas_strokeEllipse(int cx, int cy,  int a,  int b);

void Canvas_fillEllipse(int cx, int cy,  int a, int b); 

void Canvas_strokeCircle2(int cx, int cy, int r);

void Canvas_fillCircle2(int cx, int cy, int r);

void Canvas_strokeLine(int x1, int y1, int x2, int y2);

void Canvas_endDraw(void);
