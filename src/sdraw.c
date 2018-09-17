#include "../lib/dragon.h"
#include "state.h"

void stateDraw(void) {
    float matrix[4][4];
    rViewport(0, 0, s.winW, s.winH);
    Cam camera = cam((float)s.winW / (float)s.winH);
    camSetMinRect(&camera, 0, 0, s.lvl.w, s.lvl.h);
    camMatrix(&camera, matrix);
    rPipeWorld(matrix, NULL, NULL, NULL);

    float bgColor[4];
    for (int i = 0; i < 4; ++i) {
        bgColor[i] = s.color[STATE_COLOR_BACKGROUND][i] / 255.0;
    }
    rClear(bgColor);
    rDrawIndexed(R_DRAW_MODE_TRIANGLES, s.bg.ni, s.bg.i, s.bg.v);

    batchRect(&s.fg, s.hero.r, s.color[STATE_COLOR_HERO]);
    for (size_t i = 0; i < s.ejector.n; ++i) {
        if (s.ejector.arr[i].cooldown) {
            batchRect(&s.fg, s.ejector.arr[i].r, s.color[STATE_COLOR_ACTIVE_EJECTOR]);
        } else {
            batchRect(&s.fg, s.ejector.arr[i].r, s.color[STATE_COLOR_PASSIVE_EJECTOR]);
        }
    }
    for (size_t i = 0; i < s.pulsator.n; ++i) {
        CollRect r = stateOpPulsator(i);
        batchRect(&s.fg, r, s.color[STATE_COLOR_PULSATOR]);
    }
    for (size_t i = 0; i < s.shrinker.n; ++i) {
        CollRect r = stateOpShrinker(i);
        batchRect(&s.fg, r, s.color[STATE_COLOR_SHRINKER]);
    }
    for (size_t i = 0; i < s.coin.n; ++i) {
        if (!s.coin.arr[i].taken) {
            batchRect(&s.fg, s.coin.arr[i].r, s.color[STATE_COLOR_COIN]);
        }
    }
    for (size_t i = 0; i < s.graviton.n; ++i) {
        if (!s.graviton.arr[i].taken) {
            batchRect(&s.fg, s.graviton.arr[i].r, s.color[STATE_COLOR_GRAVITON]);
        }
    }
    for (size_t i = 0; i < s.key.n; ++i) {
        for (size_t j = 0; j < s.key.arr[i].key.n; ++j) {
            if (!s.key.arr[i].key.arr[j].taken) {
                batchRect(&s.fg, s.key.arr[i].key.arr[j].r, s.key.arr[i].keyColor);
            }
        }
        if (s.key.arr[i].ticksLeft == 0) {
            continue;
        }
        for (size_t j = 0; j < s.key.arr[i].lock.n; ++j) {
            CollRect r = stateOpKeyLock(i, j);
            batchRectLine(&s.fg, r, s.physics.antilockLineThickness, 0, s.key.arr[i].lockColor);
//            batchRect(&s.fg, r, s.key.arr[i].lockColor);
        }
        for (size_t j = 0; j < s.key.arr[i].antilock.n; ++j) {
            CollRect r = stateOpKeyAntilock(i, j);
            batchRectLine(&s.fg, r, s.physics.antilockLineThickness, 0, s.key.arr[i].antilockColor);
            batchRect(&s.fg, r, s.key.arr[i].antilockColor);
        }
    }
    rDrawIndexed(R_DRAW_MODE_TRIANGLES, s.fg.ni, s.fg.i, s.fg.v);
    batchClear(&s.fg);
}
