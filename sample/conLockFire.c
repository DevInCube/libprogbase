#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <progbase.h>
#include <progbase/console.h>
#include <progbase/collections/pblist.h>
#include <progbase/clock.h>

void conInit(void);
void conExit(void);
void conPrint(int x, int y, const char * str);
int bound(int value, int boundValue);
void fire(int x, int y);

int rgb2color(float r, float g, float b);
void Console_setCursorColorRgb(float red, float green, float blue, char fg);

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
            char key = getchar();
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

typedef struct { 
    float red;
    float green;
    float blue;
} Color;

Color Color_rgb(float red, float green, float blue) {
    return (Color) { red, green, blue };
}

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
const int fgcols[] = {
    FG_DEFAULT,
    FG_BLACK,
    FG_INTENSITY_BLACK,
    FG_BLUE,
    FG_MAGENTA,
    FG_RED,
    FG_INTENSITY_RED,
    FG_YELLOW,
    FG_INTENSITY_YELLOW
};
const int MAX_STATE = sizeof(cols) / sizeof(cols[0]) - 1;
const int die[] = {2, 3};
const int dieLength = sizeof(die) / sizeof(die[0]);
const int revive[] = {2};
const int reviveLength = sizeof(revive) / sizeof(revive[0]);
Color defaultColor = {1, 1, 1};

struct World {
    int w;  /* world width */
    int h;  /* world height */
    int * map;  /* current world state */
    int * tmp;  /* temporary world state (for update) */
    int * prev;  /* previous world state (for draw) */
    int mapSize; /* in bytes */
};

struct World World_init(int width, int height, int cx, int cy);
void World_fini(struct World * world);
void World_update(struct World * world);
int World_draw(struct World * world);

void fire(int x, int y) {
    const int FPS = 30;
    const double millisPerFrame = 1000 / FPS;

    struct ConsoleSize size = Console_size();
    struct World world = World_init(size.columns, size.rows * 2, x, y * 2);
    Console_clear();
    
    double elapsedMillis = 0;
    Clock lastTicks = Clock_now();
    char fpsBuf[20] = "";
    while (1) {
        Clock current = Clock_now();
        elapsedMillis = Clock_diffMillis(current, lastTicks);

        if (Console_isKeyDown()) {
            break;
        }
        World_update(&world);
        int count = World_draw(&world);
        
        double millis = Clock_diffMillis(Clock_now(), current);

        snprintf(fpsBuf, 20, "%.2f [%i]", 
            elapsedMillis < millisPerFrame ? FPS : (1000 / elapsedMillis),
            count);
        Console_setCursorAttribute(FG_BLACK);
        Console_setCursorAttribute(BG_WHITE);
        conPrint(1, 1, fpsBuf);

        if (millis < millisPerFrame) sleepMillis(millisPerFrame - millis);
        lastTicks = current;
    }
    World_fini(&world);
    Console_setCursorAttributes(BG_DEFAULT);
    Console_clear();
}

struct World World_init(int width, int height, int cx, int cy) {
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

void World_fini(struct World * world) {
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

Color getStateColor(float p) {
    Color firstCol = Color_rgb(0.3, 0, 0);
    Color secondCol = Color_rgb(1, 1, 0);
    return Color_rgb(
        firstCol.red * p + secondCol.red * (1 - p),
        firstCol.green * p + secondCol.green * (1 - p),
        firstCol.blue * p + secondCol.blue * (1 - p));
}

typedef struct {
    char * str;
    int len;
} StringBuffer;

char * StringBuffer_end(StringBuffer * self) { return self->str + self->len; }
int StringBuffer_print(StringBuffer * self) {
    write(1, self->str, sizeof(char) * self->len);
    // setvbuf(stdout, self->str, _IOFBF, self->len);
    // fflush(stdout);
    return self->len;
} 

void Console_setCursorColorRgbStr(StringBuffer * buf, Color c, char fg) {
    int col = rgb2color(c.red, c.green, c.blue);

    if (fg)
        buf->len += sprintf(StringBuffer_end(buf), "\033[38;5;%im", col);
    else
        buf->len += sprintf(StringBuffer_end(buf), "\033[48;5;%im", col);
}

typedef struct {
    int i;
    int j;
    int stateUpper;
    int stateLower;
} CellDiff;

CellDiff * CellDiff_new(int i, int j, int stateUpper, int stateLower) {
    CellDiff * self = malloc(sizeof(CellDiff));
    self->i = i;
    self->j = j;
    self->stateUpper = stateUpper;
    self->stateLower = stateLower;
    return self;
}

void drawCell(StringBuffer * buf, int stateUpper, int stateLower) {
    float pu = (MAX_STATE - stateUpper) / ((float)MAX_STATE);
    float pl = (MAX_STATE - stateLower) / ((float)MAX_STATE);
    Console_setCursorColorRgbStr(buf, ((1 - pu) < 0.1) ? defaultColor: getStateColor(pu), 1);
    Console_setCursorColorRgbStr(buf, ((1 - pl) < 0.1) ? defaultColor: getStateColor(pl), 0);
}

void drawPoint(StringBuffer * buf, int i, int j) {
    buf->len += sprintf(StringBuffer_end(buf), "\033[%i;%iH", 1 + i, 1 + j);
    buf->len += sprintf(StringBuffer_end(buf), "\xE2\x96\x80");
}

static char str[500000] = "";

int World_draw(struct World * world) {
    PbList * diffs = PbList_new();
    for (int i = 0; i < world->h; i += 2) {
        for (int j = 0; j < world->w; j++) {
            if (world->map[i * world->w + j] != world->prev[i * world->w + j]
             || world->map[(i + 1) * world->w + j] != world->prev[(i + 1) * world->w + j]) {
                 PbList_add(diffs, CellDiff_new(
                     i / 2, 
                     j, 
                     world->map[i * world->w + j], 
                     world->map[(i + 1) * world->w + j]));
            }
        }
    }
    StringBuffer * buf = &(StringBuffer){str, 0};
    // @todo create PbArrayPbList and add all diffs there
    // sort them with stateUpper + stateLower
    // apply color once for all changes that have that color combination
    for (int i = 0; i < PbList_count(diffs); i++) {
        CellDiff * diff = PbList_at(diffs, i);
        #ifdef __linux__
        {
            drawCell(buf, diff->stateUpper, diff->stateLower);
            drawPoint(buf, diff->i, diff->j);
        }
        #else
        Console_setCursorAttribute(fgcols[diff->stateUpper]);
        Console_setCursorAttribute(cols[diff->stateLower]);
        Console_setCursorPosition(diff->i, diff->j);
        printf("\xE2\x96\x80");
        #endif
    }
    while (PbList_count(diffs) > 0) {
        CellDiff * diff = PbList_at(diffs, PbList_count(diffs) - 1);
        free(diff);
        PbList_removeAt(diffs, PbList_count(diffs) - 1);
    }
    int count = StringBuffer_print(buf);
    PbList_free(diffs);
    return count;
}

void SetColor256(float r, float g, float b, char fg);

void Console_setCursorColorRgb(float red, float green, float blue, char fg) {
    SetColor256(red, green, blue, fg);
}

int rgb2color(float r, float g, float b) {
    int col = 0;

    if (g < 0) {
        if (r < 0)
            r = 0;
        else if (r > 1)
            r = 1;

        col = 232 + (int)(r * 23);
    } else {
        r = (int)(r * 5);
        g = (int)(g * 5);
        b = (int)(b * 5);

        if (r > 5)
            r = 5;
        else if (r < 0)
            r = 0;

        if (g > 5)
            g = 5;
        else if (g < 0)
            g = 0;

        if (b > 5)
            b = 5;
        else if (b < 0)
            b = 0;

        col = 16 + 36 * r + 6 * g + b;
    }
    return col;
}

// source: https://bitbucket.org/anatsanzh/progbase/src/a1c1764000bcd07850e908134c922b0b3f3f84ea/tasks/console_volumetric_rendering/raymarch_3D.c?at=master&fileviewer=file-view-defaultColort
void SetColor256(float r, float g, float b, char fg) { //0-1
    int col = rgb2color(r, g, b);

    if (fg)
        printf("\033[38;5;%im", col);
    else
        printf("\033[48;5;%im", col);
}
