#include "../lib/dragon.h"
#include "state.h"

static void batchRect(BatchDrawCall *c, Coll2Drect r, const uint8_t *color);

void stateDraw(State *state) {
    float matrix[4][4];
    rendererViewport(0, 0, state->winW, state->winH);
    Cam camera = cam((float)state->winW / (float)state->winH);
    camSetMinRect(&camera, 0, 0, state->lvl.w, state->lvl.h);
    camMatrix(&camera, matrix);
    rendererPipelineWorld(matrix, NULL, NULL, NULL);

    float bgColor[4];
    for (int i = 0; i < 4; ++i) {
        bgColor[i] = state->color[STATE_COLOR_BACKGROUND][i] / 255.0;
    }
    rendererClear(bgColor);
    rendererDrawIndexed(RENDERER_DRAW_MODE_TRIANGLES, state->bg.ni, state->bg.i, state->bg.v);

    batchRect(&state->fg, state->hero.r, state->color[STATE_COLOR_HERO]);
    for (size_t i = 0; i < state->coin.n; ++i) {
        if (!state->coin.arr[i].taken) {
            batchRect(&state->fg, state->coin.arr[i].r, state->color[STATE_COLOR_COIN]);
        }
    }
    rendererDrawIndexed(RENDERER_DRAW_MODE_TRIANGLES, state->fg.ni, state->fg.i, state->fg.v);
    batchDrawCallClear(&state->fg);
}

static void batchRect(BatchDrawCall *c, Coll2Drect r, const uint8_t *color) {
    batchDrawCallRect2D(c, r.x, r.y, r.w, r.h, color);
}
