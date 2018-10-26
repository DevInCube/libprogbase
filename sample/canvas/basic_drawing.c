#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <progbase.h>
#include <progbase/console.h>
#include <progbase/canvas.h>

int main()
{
    Console_clear();
    Canvas_setOrigin(2, 4);
    Canvas_setSize(40, 40);
    Canvas_invertYOrientation();

    Canvas_beginDraw();
    Canvas_setColorRGB(255, 0, 0);
    Canvas_putPixel(0, 0);
    Canvas_setColorRGB(255, 255, 0);
    Canvas_strokeLine(1, 1, 20, 20);
    Canvas_setColorRGB(0, 255, 255);
    Canvas_strokeCircle(20, 20, 15);
    Canvas_setColorRGB(0, 0, 255);
    Canvas_fillCircle(30, 30, 10);
    Canvas_setColorRGB(255, 0, 255);
    Canvas_fillRect(30, 30, 10, 10);
    Canvas_endDraw();
    return 0;
}