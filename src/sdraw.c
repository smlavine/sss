#include "../lib/dragon.h"
#include "state.h"

void stateDraw(State *state) {
    float matrix[4][4];
    rViewport(0, 0, state->winW, state->winH);
    Cam camera = cam((float)state->winW / (float)state->winH);
    camSetMinRect(&camera, 0, 0, state->lvl.w, state->lvl.h);
    camMatrix(&camera, matrix);
    rPipeWorld(matrix, NULL, NULL, NULL);

    float bgColor[4];
    for (int i = 0; i < 4; ++i) {
        bgColor[i] = state->color[STATE_COLOR_BACKGROUND][i] / 255.0;
    }
    rClear(bgColor);
    rDrawIndexed(R_DRAW_MODE_TRIANGLES, state->bg.ni, state->bg.i, state->bg.v);

    batchCallRect(&state->fg, state->hero.r, state->color[STATE_COLOR_HERO]);
    for (size_t i = 0; i < state->ejector.n; ++i) {
        if (state->ejector.arr[i].cooldown) {
            batchCallRect(&state->fg, state->ejector.arr[i].r, state->color[STATE_COLOR_ACTIVE_EJECTOR]);
        } else {
            batchCallRect(&state->fg, state->ejector.arr[i].r, state->color[STATE_COLOR_PASSIVE_EJECTOR]);
        }
    }
    for (size_t i = 0; i < state->pulsator.n; ++i) {
        CollRect r = stateOpPulsator(state, i);
        batchCallRect(&state->fg, r, state->color[STATE_COLOR_PULSATOR]);
    }
    for (size_t i = 0; i < state->shrinker.n; ++i) {
        CollRect r = stateOpShrinker(state, i);
        batchCallRect(&state->fg, r, state->color[STATE_COLOR_SHRINKER]);
    }
    for (size_t i = 0; i < state->coin.n; ++i) {
        if (!state->coin.arr[i].taken) {
            batchCallRect(&state->fg, state->coin.arr[i].r, state->color[STATE_COLOR_COIN]);
        }
    }
    for (size_t i = 0; i < state->key.n; ++i) {
        for (size_t j = 0; j < state->key.arr[i].key.n; ++j) {
            if (!state->key.arr[i].key.arr[j].taken) {
                batchCallRect(&state->fg, state->key.arr[i].key.arr[j].r, state->key.arr[i].keyColor);
            }
        }
        if (state->key.arr[i].ticksLeft == 0) {
            continue;
        }
        for (size_t j = 0; j < state->key.arr[i].lock.n; ++j) {
            CollRect r = stateOpKeyLock(state, i, j);
            batchCallRect(&state->fg, r, state->key.arr[i].lockColor);
        }
    }
    rDrawIndexed(R_DRAW_MODE_TRIANGLES, state->fg.ni, state->fg.i, state->fg.v);
    batchCallClear(&state->fg);
}
