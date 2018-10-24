#pragma once 

void Canvas_setOrigin(int conRow, int conColumn);

void Canvas_setSize(int widthPixels, int heightPixels);

void Canvas_invertYOrientation(void);

void Canvas_setColorRGB(int red, int green, int blue);

void Canvas_beginDraw(void);

void Canvas_putPixel(int x, int y);

void Canvas_putLine(int x1, int y1, int x2, int y2);

void Canvas_putCircle(int x0, int y0, int radius);

void Canvas_endDraw(void);
