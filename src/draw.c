#include "../lib/dragon.h"
#include "s.h"

// TODO: make key, lock, antilock colors more distinguishable
#define COLOR_HERO            (const uint8_t[]) { 255,   0,   0, 255 }
#define COLOR_ACTIVE_EJECTOR  (const uint8_t[]) { 255, 128, 128, 255 }
#define COLOR_PASSIVE_EJECTOR (const uint8_t[]) { 128,   0,   0, 255 }
#define COLOR_PULSATOR        (const uint8_t[]) {   0,   0,   0, 255 }
#define COLOR_SHRINKER        (const uint8_t[]) { 192, 192, 192, 255 }
#define COLOR_COIN            (const uint8_t[]) { 255, 255,   0, 255 }
#define COLOR_GRAVITON        (const uint8_t[]) {   0,   0, 255, 255 }
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
#define CLEAR_COLOR (const float[]){1, 1, 1, 1}

void sDraw(int winW, int winH) {
    rViewport(0, 0, winW, winH);

    float matrix[4][4];
    Cam camera = cam((float)winW / (float)winH);
    camSetMinRect(&camera, 0, 0, s.lvl.w, s.lvl.h);
    camMatrix(&camera, matrix);
    rPipeWorld(matrix, NULL, NULL, NULL);

    rClear(CLEAR_COLOR);

    rDrawIndexed(R_DRAW_MODE_TRIANGLES,s.draw.bg.ni,s.draw.bg.i,s.draw.bg.v);

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

    batchRect(&s.draw.fg, s.hero.r, COLOR_HERO);

    rDrawIndexed(R_DRAW_MODE_TRIANGLES,s.draw.fg.ni,s.draw.fg.i,s.draw.fg.v);

    batchClear(&s.draw.fg);
}
