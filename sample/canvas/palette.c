#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <progbase.h>
#include <progbase/console.h>
#include <progbase/canvas.h>

int main()
{
    Console_clear();
    Canvas_setSize(36, 8);

    Canvas_beginDraw();
    //
    for (int i = 0; i < 16; i++)
    {
        Canvas_setColorValue(i);
        Canvas_putPixel(i, 0);
    }
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 36; j++)
        {
            Canvas_setColorValue(16 + i * 36 + j);
            Canvas_putPixel(j, i + 1);
        }
    }
    for (int i = 0; i < 24; i++)
    {
        Canvas_setColorValue(16 + 6 * 36 + i);
        Canvas_putPixel(i, 7);
    }
    //
    Canvas_endDraw();
    puts("");
    return 0;
}
