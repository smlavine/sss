#include "s.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#define P_HERO                0xff0000
#define P_NONE                0xffffff
#define P_WALL                0x000000
#define P_EJECTOR             0x800000
#define P_PULSATOR_CONTRACTED 0x787878
#define P_PULSATOR_EXPANDED   0x78787a
#define P_SHRINKER            0xc0c0c0
#define P_COIN                0xffff00
#define P_GRAVITON            0x0000ff

#define KEY_PIXEL_COUNT 4
#define P_KEY          (const int[]) {0x008001, 0x008002, 0x008003, 0x008004}
#define P_KEY_LOCK     (const int[]) {0x00ff01, 0x00ff02, 0x00ff03, 0x00ff04}
#define P_KEY_ANTILOCK (const int[]) {0x01ff00, 0x02ff00, 0x03ff00, 0x04ff00}

#define WALL_COLOR (const uint8_t[]) {0, 0, 0, 0}
#define PULSATOR_CONTRACTED_OFFSET 0
#define PULSATOR_EXPANDED_OFFSET 60
#define PLAT_SPEED 10 // the bigger the slower; how many ticks to cross a tile

typedef struct {
    int x, y, w, h;
} Rect;

typedef struct {
    size_t n;
    Rect *arr;
} RectArr;

typedef struct {
    int w, h;
    size_t n;
    struct {
        float x, y;
    } *arr;
} Plat;

typedef enum {
    DIR_WEST,
    DIR_EAST,
    DIR_SOUTH,
    DIR_NORTH
} Dir;
#define P_DIR  (const int[]){0xfefefe, 0xfdfdfd, 0xfcfcfc, 0xfbfbfb}
#define P_PLAT (const int[]){0x010101, 0x020202, 0x030303, 0x040404}

struct S s;

static void skipComments(FILE *f, bool skipLeadingWhitespaceToo);
static RectArr getRectArr(int w, int h, int *p, int px, int d);
static Plat getPlat(Bmp tab, const int *p, Rect r, Dir d);

void sLoad(const char *path) {
    int w, h, dump;
    FILE *f = fopen(path, "rb");
    fscanf(f, "P6");
    skipComments(f, true);
    fscanf(f, "%d", &w);
    skipComments(f, true);
    fscanf(f, "%d", &h);
    skipComments(f, true);
    fscanf(f, "%d", &dump);
    skipComments(f, false);
    fread(&dump, 1, 1, f);
    uint8_t *b = malloc(w * h * 3);
    fread(b, w * h * 3, 1, f);
    int *p = malloc(w * h * sizeof(*p));
    for (int i = 0; i < w * h; ++i) {
        int i2 = (h - i / w - 1) * w + (i % w);
        p[i2] = (b[i * 3] << 16) + (b[i * 3 + 1] << 8) + b[i * 3 + 2];
    }
    free(b);
    fclose(f);

    memset(&s, 0, sizeof(s));

    RectArr hero = getRectArr(w, h, p, P_HERO, P_NONE);
    s.hero.n = hero.n;
    s.hero.i = 0;
    s.hero.arr = malloc(s.hero.n * sizeof(*s.hero.arr));
    for (size_t i = 0; i < s.hero.n; ++i) {
        s.hero.arr[i].x = hero.arr[i].x;
        s.hero.arr[i].y = hero.arr[i].y;
        s.hero.arr[i].w = hero.arr[i].w;
        s.hero.arr[i].h = hero.arr[i].h;
    }
    free(hero.arr);

    s.lvl = (Bmp){w, h, malloc(w * h * sizeof(*s.lvl.b))};
    for (int i = 0; i < w * h; ++i) {
        bool bit = (p[i] == P_WALL || p[i] == P_EJECTOR);
        bmpSet(s.lvl, i % w, i / w, bit);
    }

    s.tab = (Bmp){w, h, calloc(w * h, sizeof(*s.lvl.b))};

    RectArr wall = getRectArr(w, h, p, P_WALL, P_NONE);
    for (size_t i = 0; i < wall.n; ++i) {
        Rect r = wall.arr[i];
        batchRect(&s.draw.bg, (CollRect){r.x, r.y, r.w, r.h}, WALL_COLOR);
    }
    free(wall.arr);

    RectArr ejector = getRectArr(w, h, p, P_EJECTOR, P_NONE);
    s.ejector.n = ejector.n;
    s.ejector.arr = calloc(s.ejector.n, sizeof(*s.ejector.arr));
    for (size_t i = 0; i < s.ejector.n; ++i) {
        Rect r = ejector.arr[i];
        s.ejector.arr[i].r = (CollRect){r.x, r.y, r.w, r.h};
    }
    free(ejector.arr);

    RectArr pulsatorC = getRectArr(w, h, p, P_PULSATOR_CONTRACTED, P_NONE);
    RectArr pulsatorE = getRectArr(w, h, p, P_PULSATOR_EXPANDED, P_NONE);
    s.pulsator.n = pulsatorC.n + pulsatorE.n;
    s.pulsator.arr = malloc(s.pulsator.n * sizeof(*s.pulsator.arr));
    for (size_t i = 0; i < pulsatorC.n; ++i) {
        Rect r = pulsatorC.arr[i];
        bmpSetR(s.tab, r.x, r.y, r.w, r.h, true);
        s.pulsator.arr[i].offset = PULSATOR_CONTRACTED_OFFSET;
        s.pulsator.arr[i].r = (CollRect){r.x, r.y, r.w, r.h};
    }
    for (size_t i = 0; i < pulsatorE.n; ++i) {
        Rect r = pulsatorE.arr[i];
        bmpSetR(s.tab, r.x, r.y, r.w, r.h, true);
        s.pulsator.arr[i + pulsatorC.n].offset = PULSATOR_EXPANDED_OFFSET;
        s.pulsator.arr[i + pulsatorC.n].r = (CollRect){r.x, r.y, r.w, r.h};
    }
    free(pulsatorC.arr);
    free(pulsatorE.arr);

    RectArr shrinker = getRectArr(w, h, p, P_SHRINKER, P_NONE);
    s.shrinker.n = shrinker.n;
    s.shrinker.arr = malloc(s.shrinker.n * sizeof(*s.shrinker.arr));
    for (size_t i = 0; i < s.shrinker.n; ++i) {
        Rect r = shrinker.arr[i];
        bmpSetR(s.tab, r.x, r.y, r.w, r.h, true);
        s.shrinker.arr[i].ticksLeft = -1;
        s.shrinker.arr[i].r = (CollRect){r.x, r.y, r.w, r.h};
    }
    free(shrinker.arr);

    RectArr coin = getRectArr(w, h, p, P_COIN, P_NONE);
    s.coin.n = coin.n;
    s.coin.arr = calloc(s.coin.n, sizeof(*s.coin.arr));
    for (size_t i = 0; i < s.coin.n; ++i) {
        Rect r = coin.arr[i];
        s.coin.arr[i].r = (CollRect){r.x, r.y, r.w, r.h};
    }
    free(coin.arr);

    RectArr graviton = getRectArr(w, h, p, P_GRAVITON, P_NONE);
    s.graviton.n = graviton.n;
    s.graviton.arr = calloc(s.graviton.n, sizeof(*s.graviton.arr));
    for (size_t i = 0; i < s.graviton.n; ++i) {
        Rect r = graviton.arr[i];
        s.graviton.arr[i].r = (CollRect){r.x, r.y, r.w, r.h};
    }
    free(graviton.arr);

    RectArr key[KEY_PIXEL_COUNT];
    RectArr keyLock[KEY_PIXEL_COUNT];
    RectArr keyAntilock[KEY_PIXEL_COUNT];
    for (int i = 0; i < KEY_PIXEL_COUNT; ++i) {
        key[i] = getRectArr(w, h, p, P_KEY[i], P_NONE);
        keyLock[i] = getRectArr(w, h, p, P_KEY_LOCK[i], P_NONE);
        keyAntilock[i] = getRectArr(w, h, p, P_KEY_ANTILOCK[i], P_NONE);
        if (key[i].n > 0) {
            s.key.arr = realloc(s.key.arr, ++s.key.n * sizeof(*s.key.arr));
            s.key.arr[i].ticksLeft = -1;
            s.key.arr[i].n = key[i].n;
            s.key.arr[i].arr=calloc(s.key.arr[i].n,sizeof(*s.key.arr[i].arr));
            for (size_t j = 0; j < s.key.arr[i].n; ++j) {
                Rect r = key[i].arr[j];
                s.key.arr[i].arr[j].r = (CollRect){r.x, r.y, r.w, r.h};
            }
            s.key.arr[i].lock.n = keyLock[i].n;
            s.key.arr[i].lock.arr = malloc(s.key.arr[i].lock.n
                                    * sizeof(*s.key.arr[i].lock.arr));
            for (size_t j = 0; j < s.key.arr[i].lock.n; ++j) {
                Rect r = keyLock[i].arr[j];
                bmpSetR(s.tab, r.x, r.y, r.w, r.h, true);
                s.key.arr[i].lock.arr[j] = (CollRect){r.x, r.y, r.w, r.h};
            }
            s.key.arr[i].antilock.n = keyAntilock[i].n;
            s.key.arr[i].antilock.arr = malloc(s.key.arr[i].antilock.n
                                        * sizeof(*s.key.arr[i].antilock.arr));
            for (size_t j = 0; j < s.key.arr[i].antilock.n; ++j) {
                Rect r = keyAntilock[i].arr[j];
                bmpSetR(s.tab, r.x, r.y, r.w, r.h, true);
                s.key.arr[i].antilock.arr[j] = (CollRect){r.x, r.y, r.w, r.h};
            }
        }
        free(key[i].arr);
        free(keyLock[i].arr);
        free(keyAntilock[i].arr);
    }

    RectArr platWest = getRectArr(w, h, p, P_PLAT[DIR_WEST], P_NONE);
    RectArr platEast = getRectArr(w, h, p, P_PLAT[DIR_EAST], P_NONE);
    RectArr platSouth = getRectArr(w, h, p, P_PLAT[DIR_SOUTH], P_NONE);
    RectArr platNorth = getRectArr(w, h, p, P_PLAT[DIR_NORTH], P_NONE);
    size_t platN = platWest.n + platEast.n + platSouth.n + platNorth.n;
    Plat *plat = malloc(platN * sizeof(*plat));
    for (size_t i = 0; i < platWest.n; ++i) {
        Plat pl = getPlat(s.tab, p, platWest.arr[i], DIR_WEST);
        plat[i] = pl;
    }
    for (size_t i = 0; i < platEast.n; ++i) {
        Plat pl = getPlat(s.tab, p, platEast.arr[i], DIR_EAST);
        plat[i + platWest.n] = pl;
    }
    for (size_t i = 0; i < platSouth.n; ++i) {
        Plat pl = getPlat(s.tab, p, platSouth.arr[i], DIR_SOUTH);
        plat[i + platWest.n + platEast.n] = pl;
    }
    for (size_t i = 0; i < platNorth.n; ++i) {
        Plat pl = getPlat(s.tab, p, platNorth.arr[i], DIR_NORTH);
        plat[i + platWest.n + platEast.n + platSouth.n] = pl;
    }
    s.plat.n = platN;
    s.plat.arr = malloc(platN * sizeof(*s.plat.arr));
    for (size_t i = 0; i < platN; ++i) {
        s.plat.arr[i].w = plat[i].w;
        s.plat.arr[i].h = plat[i].h;
        s.plat.arr[i].n = plat[i].n;
        s.plat.arr[i].arr = malloc(plat[i].n * sizeof(*plat[i].arr));
        for (size_t j = 0; j < plat[i].n; ++j) {
            s.plat.arr[i].arr[j].x = plat[i].arr[j].x;
            s.plat.arr[i].arr[j].y = plat[i].arr[j].y;
        }
    }
    free(plat);
    free(platWest.arr);
    free(platEast.arr);
    free(platSouth.arr);
    free(platNorth.arr);

    free(p);

    for (int i = 0; i < 6; ++ i) {
        s.releasekUpLeftRightRShftTab[i] = true;
    }

    s.hero.arr[s.hero.i].y -= 0.001;
    sOpEnvEnergy(&s.hero.envVelX, &s.hero.envVelY);
}

void sFree(void) {
    batchDel(&s.draw.bg, false);
    batchDel(&s.draw.fg, false);
    free(s.lvl.b);
    free(s.tab.b);
    free(s.hero.arr);
    free(s.ejector.arr);
    free(s.pulsator.arr);
    free(s.shrinker.arr);
    free(s.coin.arr);
    free(s.graviton.arr);
    for (size_t i = 0; i < s.key.n; ++i) {
        free(s.key.arr[i].arr);
        free(s.key.arr[i].lock.arr);
        free(s.key.arr[i].antilock.arr);
    }
    free(s.key.arr);
    for (size_t i = 0; i < s.plat.n; ++i) {
        free(s.plat.arr[i].arr);
    }
    free(s.plat.arr);
}

static void skipComments(FILE *f, bool skipLeadingWhitespaceToo) {
    int c = fgetc(f);
    while (c == '#' || (isspace(c) && skipLeadingWhitespaceToo)) {
        if (c == '#') {
            while (c != '\n') {
                c = fgetc(f);
            }
        }
        c = fgetc(f);
    }
    ungetc(c, f);
}

static RectArr getRectArr(int w, int h, int *p, int px, int d) {
    size_t m = 16;
    RectArr r = {0, malloc(m * sizeof(*r.arr))};

    for (int x = 0; x < w; ++x) {
        for (int y = 0; y < h; ++y) {
            if (p[y * w + x] == px) {
                int rw = 1;
                int rh = 1;

                while (x + rw < w && p[y * w + x + rw] == px) {
                    ++rw;
                }

                while (y + rh < h) {
                    for (int rx = x; rx < x + rw; ++rx) {
                        if (p[(y + rh) * w + rx] != px) {
                            goto rect;
                        }
                    }
                    ++rh;
                }

            rect:

                for (int X = x; X < x + rw; ++X) {
                    for (int Y = y; Y < y + rh; ++Y) {
                        p[Y * w + X] = d;
                    }
                }

                r.arr[r.n] = (Rect){x, y, rw, rh};
                ++r.n;

                if (r.n == m) {
                    m *= 2;
                    r.arr = realloc(r.arr, m * sizeof(*r.arr));
                }
            }
        }
    }

    return r;
}

static Plat getPlat(Bmp tab, const int *p, Rect r, Dir d) {
    size_t m = 16;
    Plat plat = {r.w, r.h, 0, malloc(m * sizeof(*plat.arr))};

    int vx = 0;
    int vy = 0;
    switch (d) {
        case DIR_WEST: vx = -1; break;
        case DIR_EAST: vx =  1; break;
        case DIR_SOUTH: vy = -1; break;
        case DIR_NORTH: vy =  1; break;
        default: break;
    }

    int x = r.x;
    int y = r.y;
    do {

        int newX = x + vx;
        int newY = y + vy;
        for (int xi = newX; xi < newX + r.w; ++xi) {
            for (int yi = newY; yi < newY + r.h; ++yi) {
                int px = p[yi * tab.w + xi];
                switch (px) {
                    case P_DIR[DIR_WEST]:
                        vx = -1;
                        vy =  0;
                        break;
                    case P_DIR[DIR_EAST]:
                        vx =  1;
                        vy =  0;
                        break;
                    case P_DIR[DIR_SOUTH]:
                        vx =  0;
                        vy = -1;
                        break;
                    case P_DIR[DIR_NORTH]:
                        vx =  0;
                        vy =  1;
                        break;
                    default:
                        break;
                }
            }
        }

        newX = x + vx;
        newY = y + vy;

        bmpSetR(tab, newX, newY, r.w, r.h, true);

        for (int i = 0; i < PLAT_SPEED; ++i) {
            plat.arr[plat.n].x = x + (float)vx * (float)i / (float)PLAT_SPEED;
            plat.arr[plat.n].y = y + (float)vy * (float)i / (float)PLAT_SPEED;

            ++plat.n;

            if (plat.n == m) {
                m *= 2;
                plat.arr = realloc(plat.arr, m * sizeof(*plat.arr));
            }
        }

        x = newX;
        y = newY;

    } while (x != r.x || y != r.y);

    return plat;
}
