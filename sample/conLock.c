#include <stdio.h>
#include <stdlib.h>

#include <progbase/console.h>

int main(void) {
    Console_lockInput();
    printf("Use [WASD] to move, [Q] to quit: ");
    while (1) {
        char key = getchar();
        if (key == 'q') break;
        if (key == 'w') printf("[UP] ");
        if (key == 'a') printf("[LEFT] ");
        if (key == 's') printf("[DOWN] ");
        if (key == 'd') printf("[RIGHT] ");
    }
    Console_unlockInput();
    return 0;
}
