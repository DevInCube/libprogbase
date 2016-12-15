#include <stdio.h>
#include <stdlib.h>
#include <pbconsole.h>

int main(void) {
    conLockInput();
    while (1) {
        char key = getchar();
        if (key == 'q') break;
        if (key == 'w') printf("[UP] ");
        if (key == 'a') printf("[LEFT] ");
        if (key == 's') printf("[RIGHT] ");
        if (key == 'd') printf("[DOWN] ");
    }
    conUnlockInput();
    return 0;
}
