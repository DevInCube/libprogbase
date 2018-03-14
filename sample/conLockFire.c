#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <progbase.h>
#include <progbase/console.h>

void conInit(void);
void conExit(void);
void conPrint(int x, int y, const char * str);
int bound(int value, int boundValue);
void fire(int x, int y);

int main(void) {
    const char * HELP = "Use [WASD] to control, [Space] to fire, [Q] - to quit:";
    struct ConsoleSize size = Console_size();
    int px = size.columns / 2;
    int py = size.rows / 2;
    conInit();
    while (1) {
        conPrint(1, 1, HELP);
        conPrint(px, py, " ");  /* clear previous position */
        if (Console_isKeyDown()) {
            char key = Console_getChar();
            if (key == 'q') break;
            else if (key == 'w') py = bound(py - 1, size.rows);
            else if (key == 'a') px = bound(px - 1, size.columns);
            else if (key == 's') py = bound(py + 1, size.rows);
            else if (key == 'd') px = bound(px + 1, size.columns);
            else if (key == ' ') fire(px, py);
        }
        conPrint(px, py, "#");  /* draw new position */
        sleepMillis(33);  /* ~ 30 FPS */
    }
    conExit();
    return 0;
}

int bound(int value, int boundValue) {
    return (boundValue + value) % boundValue;
}

char conGetCharAndClear(void) {
    char key = Console_getChar();
    char ch;
    while((ch = Console_getChar()) != EOF && ch != '\n') {};
    return key;
}

void conPrint(int x, int y, const char * str) {
    Console_setCursorPosition(y, x);
    printf("%s", str);
    fflush(stdout);
}

void conInit(void) {
    Console_lockInput();
    Console_hideCursor();
    Console_clear();
}

void conExit(void) {
    Console_showCursor();
    Console_unlockInput();
    Console_setCursorPosition(1, 1);
    Console_clear();
}

/* FIRE */

const int cols[] = {
    BG_DEFAULT,
    BG_BLACK,
    BG_INTENSITY_BLACK,
    BG_BLUE,
    BG_MAGENTA,
    BG_RED,
    BG_INTENSITY_RED,
    BG_YELLOW,
    BG_INTENSITY_YELLOW
};
const int MAX_STATE = sizeof(cols) / sizeof(cols[0]) - 1;
const int die[] = {2, 3};
const int dieLength = sizeof(die) / sizeof(die[0]);
const int revive[] = {2};
const int reviveLength = sizeof(revive) / sizeof(revive[0]);

struct World {
    int w;  /* world width */
    int h;  /* world height */
    int * map;  /* current world state */
    int * tmp;  /* temporary world state (for update) */
    int * prev;  /* previous world state (for draw) */
    int mapSize; /* in bytes */
};

struct World World_new(int width, int height, int cx, int cy);
void World_free(struct World * world);
void World_update(struct World * world);
void World_draw(struct World * world);

void fire(int x, int y) {
    struct ConsoleSize size = Console_size();
    struct World world = World_new(size.columns, size.rows, x, y);
    Console_clear();
    while (1) {
        if (Console_isKeyDown()) {
            char key = Console_getChar();
            if (key != EOF) break;
        }
        World_update(&world);
        World_draw(&world);
        sleepMillis(33);  /* ~ 30 FPS */
    }
    World_free(&world);
    Console_setCursorAttributes(BG_DEFAULT);
    Console_clear();
}

struct World World_new(int width, int height, int cx, int cy) {
    struct World world;
    world.w = width;
    world.h = height;
    world.mapSize = sizeof(int) * world.h * world.w;
    world.map = malloc(world.mapSize);
    world.tmp = malloc(world.mapSize);
    world.prev = malloc(world.mapSize);
    memset(world.map, 0, world.mapSize);
    /* Glyder */
    /*world.map[(cy - 1) * world.w + (cx - 1)] = MAX_STATE;
    world.map[(cy) * world.w + (cx - 1)] = MAX_STATE;
    world.map[(cy + 1) * world.w + (cx - 1)] = MAX_STATE;
    world.map[(cy + 1) * world.w + (cx)] = MAX_STATE;
    world.map[(cy + 0) * world.w + (cx + 1)] = MAX_STATE;*/
    world.map[cy * world.w + cx] = MAX_STATE;
    world.map[(cy + 1) * world.w + cx] = MAX_STATE;
    world.map[(cy + 1) * world.w + (cx + 1)] = MAX_STATE;
    return world;
}

void World_free(struct World * world) {
    free(world->map);
    free(world->tmp);
    free(world->prev);
}

int countNeighbours(int i, int j, struct World * world) {
    int count = 0;
    int si = 0;
    int sj = 0;
    for (si = -1; si <= 1; si ++) {
        for (sj = -1; sj <= 1; sj ++) {
            int ci = i + si;
            int cj = j + sj;
            if (si == sj && si == 0) continue;
            if ((ci < 0 || ci >= world->h) || (cj < 0 || cj >= world->w)) continue;
            if (world->map[ci * world->w + cj] == MAX_STATE) {
                count++;
            }
        }
    }
    return count;
}

void World_update(struct World * world) {
    int i;
    int j;
    memcpy(world->prev, world->map, world->mapSize);
    memcpy(world->tmp, world->map,world-> mapSize);
    for (i = 0; i < world->h; i++) {
        for (j = 0; j < world->w; j++) {
            int c = world->map[i * world->w + j];
            if (c == 0 || c == MAX_STATE) {
                int count = countNeighbours(i, j, world);
                if (c == MAX_STATE) {
                    int dies = 1;
                    int d = 0;
                    for (d = 0; d < dieLength; d++) {
                        if (die[d] == count) {
                            dies = 0;
                            break;
                        }
                    }
                    if (dies) c--;
                }
                if (c == 0) {
                    int revives = 0;
                    int d = 0;
                    for (d = 0; d < reviveLength; d++) {
                        if (revive[d] == count) {
                            revives = 1;
                            break;
                        }
                    }
                    if (revives) c = MAX_STATE;
                }
            } else {
                c--;
            }
            world->tmp[i * world->w + j] = c;
        }
    }
    memcpy(world->map, world->tmp, world->mapSize);
}

void drawCell(int i, int j, int state) {
    int col = cols[state];
    Console_setCursorAttribute(col);
    conPrint(1 + j, 1 + i, " ");
}

void World_draw(struct World * world) {
    int i = 0;
    int j = 0;
    for (i = 0; i < world->h; i++) {
        for (j = 0; j < world->w; j++) {
            if (world->map[i * world->w + j] != world->prev[i * world->w + j]) {
                drawCell(i, j, world->map[i * world->w + j]);
            }
        }
    }
}
