#include "s.h"

#define COLOR_ACTIVE_HERO     (const uint8_t[]) { 255,   0,   0, 255 }
#define COLOR_PASSIVE_HERO    (const uint8_t[]) { 128,   0,   0, 255 }
#define COLOR_ACTIVE_EJECTOR  (const uint8_t[]) { 255, 128, 128, 255 }
#define COLOR_PASSIVE_EJECTOR (const uint8_t[]) { 128,   0,   0, 255 }
#define COLOR_PULSATOR        (const uint8_t[]) {   0,   0,   0, 255 }
#define COLOR_SHRINKER        (const uint8_t[]) { 192, 192, 192, 255 }
#define COLOR_COIN            (const uint8_t[]) { 255, 255,   0, 255 }
#define COLOR_GRAVITON        (const uint8_t[]) {   0,   0, 255, 255 }
#define COLOR_PLAT            (const uint8_t[]) {   0,   0,   0, 255 }
#define COLOR_KEY             (const uint8_t[][4]) { {   0, 128,   0, 255 }, \
                                                     {   0, 128,   0, 255 }, \
                                                     {   0, 128,   0, 255 }, \
                                                     {   0, 128,   0, 255 } }
#define COLOR_KEY_LOCK        (const uint8_t[][4]) { {   0, 255,   0, 255 }, \
                                                     {   0, 255,   0, 255 }, \
                                                     {   0, 255,   0, 255 }, \
                                                     {   0, 255,   0, 255 } }
#define COLOR_KEY_ANTILOCK    (const uint8_t[][4]) { {   0, 255,   0, 255 }, \
                                                     {   0, 255,   0, 255 }, \
                                                     {   0, 255,   0, 255 }, \
                                                     {   0, 255,   0, 255 } }
#define KEY_ANTILOCK_LINE_THICKNESS 0.1
#define CLEAR_COLOR 255, 255, 255

void sDraw(int winW, int winH) {
    rViewport(0, 0, winW, winH);
    float ar = (float)winW / winH;
    if (ar > (float)s.lvl.w / s.lvl.h) {
        float mul = 2.0 / s.lvl.h;
        float tilesPerUnit = s.lvl.h / 2.0;
        float offset = -(s.lvl.w / tilesPerUnit) / (2 * ar);
        rPipe(mul / ar, mul, offset, -1);
    } else {
        float mul = 2.0 * ar / s.lvl.w;
        float tilesPerUnit = s.lvl.w / (ar * 2);
        float offset = -(s.lvl.h / tilesPerUnit) / 2;
        rPipe(mul / ar, mul, -1, offset);
    }

    rClear(CLEAR_COLOR);

    rDrawIndexedTriangles(s.draw.bg.ni,s.draw.bg.i,s.draw.bg.v);

    for (size_t i = 0; i < s.plat.n; ++i) {
        batchRect(&s.draw.fg, sOpPlat(i), COLOR_PLAT);
    }

    for (size_t i = 0; i < s.ejector.n; ++i) {
        if (s.ejector.arr[i].cooldown) {
            batchRect(&s.draw.fg, s.ejector.arr[i].r, COLOR_ACTIVE_EJECTOR);
        } else {
            batchRect(&s.draw.fg, s.ejector.arr[i].r, COLOR_PASSIVE_EJECTOR);
        }
    }

    for (size_t i = 0; i < s.pulsator.n; ++i) {
        CollRect r = sOpPulsator(i);
        batchRect(&s.draw.fg, r, COLOR_PULSATOR);
    }

    for (size_t i = 0; i < s.shrinker.n; ++i) {
        CollRect r = sOpShrinker(i);
        batchRect(&s.draw.fg, r, COLOR_SHRINKER);
    }

    for (size_t i = 0; i < s.coin.n; ++i) {
        if (!s.coin.arr[i].taken) {
            batchRect(&s.draw.fg, s.coin.arr[i].r, COLOR_COIN);
        }
    }

    for (size_t i = 0; i < s.graviton.n; ++i) {
        if (!s.graviton.arr[i].taken) {
            batchRect(&s.draw.fg, s.graviton.arr[i].r, COLOR_GRAVITON);
        }
    }

    for (size_t i = 0; i < s.key.n; ++i) {
        for (size_t j = 0; j < s.key.arr[i].n; ++j) {
            if (!s.key.arr[i].arr[j].taken) {
                batchRect(&s.draw.fg, s.key.arr[i].arr[j].r, COLOR_KEY[i]);
            }
        }

        for (size_t j = 0; j < s.key.arr[i].lock.n; ++j) {
            CollRect r = sOpKeyLock(i, j);
            batchRect(&s.draw.fg, r, COLOR_KEY_LOCK[i]);
        }

        for (size_t j = 0; j < s.key.arr[i].antilock.n; ++j) {
            CollRect r = sOpKeyAntilock(i, j);
            float t = KEY_ANTILOCK_LINE_THICKNESS;
            batchRectLine(&s.draw.fg, s.key.arr[i].antilock.arr[j],
                          t, 0, COLOR_KEY_ANTILOCK[i]);
            batchRect(&s.draw.fg, r, COLOR_KEY_ANTILOCK[i]);
        }
    }

    for (size_t i = 0; i < s.hero.n; ++i) {
        if (i == s.hero.i) {
            batchRect(&s.draw.fg, s.hero.arr[i].r, COLOR_ACTIVE_HERO);
        } else {
            batchRect(&s.draw.fg, s.hero.arr[i].r, COLOR_PASSIVE_HERO);
        }
    }

    rDrawIndexedTriangles(s.draw.fg.ni,s.draw.fg.i,s.draw.fg.v);

    batchClear(&s.draw.fg);
}
