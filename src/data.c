#include "../lib/dragon.h"
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

#define PORTAL_PIXEL_COUNT 4
#define P_PORTAL (const int[]) {0xff00ff, 0xff01ff, 0xff02ff, 0xff03ff}

#define WALL_COLOR (const uint8_t[]) {0, 0, 0, 0}
#define PULSATOR_CONTRACTED_OFFSET 0
#define PULSATOR_EXPANDED_OFFSET 60

typedef struct {
    int x, y, w, h;
} Rect;

typedef struct {
    size_t n;
    Rect *arr;
} RectArr;

struct S s;

static void skipComments(FILE *f, bool skipLeadingWhitespaceToo);
static RectArr getRectArr(int w, int h, int *p, int px, int d);

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

    for (int i = 0; i < w * h; ++i) {
        if (p[i] == P_HERO) {
            s.hero.r.x = i % w;
            s.hero.r.y = i / w;
            s.hero.r.w = 1;
            s.hero.r.h = 1;
            break;
        }
    }

    bmpNew(w, h, 1, &s.lvl);
    for (int i = 0; i < w * h; ++i) {
        bool bit = (p[i] == P_WALL || p[i] == P_EJECTOR);
        bmpSet(&s.lvl, i % w, i / w, 0, bit);
    }

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
        s.pulsator.arr[i].offset = PULSATOR_CONTRACTED_OFFSET;
        s.pulsator.arr[i].r = (CollRect){r.x, r.y, r.w, r.h};
    }
    for (size_t i = 0; i < pulsatorE.n; ++i) {
        Rect r = pulsatorE.arr[i];
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
                s.key.arr[i].lock.arr[j] = (CollRect){r.x, r.y, r.w, r.h};
            }
            s.key.arr[i].antilock.n = keyAntilock[i].n;
            s.key.arr[i].antilock.arr = malloc(s.key.arr[i].antilock.n
                                        * sizeof(*s.key.arr[i].antilock.arr));
            for (size_t j = 0; j < s.key.arr[i].antilock.n; ++j) {
                Rect r = keyAntilock[i].arr[j];
                s.key.arr[i].antilock.arr[j] = (CollRect){r.x, r.y, r.w, r.h};
            }
        }
        free(key[i].arr);
        free(keyLock[i].arr);
        free(keyAntilock[i].arr);
    }

    RectArr portal[PORTAL_PIXEL_COUNT];
    for (int i = 0; i < PORTAL_PIXEL_COUNT; ++i) {
        portal[i] = getRectArr(w, h, p, P_PORTAL[i], P_NONE);
        Rect *r = portal[i].arr;
        s.portal.arr=realloc(s.portal.arr,++s.portal.n*sizeof(*s.portal.arr));
        s.portal.arr[s.portal.n-1].a=(CollRect){r[0].x,r[0].y,r[0].h,r[0].w};
        s.portal.arr[s.portal.n-1].b=(CollRect){r[1].x,r[1].y,r[1].h,r[1].w};
        free(portal[i].arr);
    }

    free(p);

    // TODO: environmental velocity
}

void sFree(void) {
    batchDel(&s.draw.bg, false);
    batchDel(&s.draw.fg, false);
    bmpDel(&s.lvl, false);
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
