#include <stdbool.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "matrix.h"
#include "camera.h"
#include "image.h"
#include "renderer.h"
#include "batch.h"
#include "state.h"

#define SCAN_ARRAY(f, item, fn) \
    fscanf(f, "%zu", &item.n); \
    item.arr = malloc(item.n * sizeof(*item.arr)); \
    for (size_t i = 0; i < item.n; ++i) { \
        item.arr[i] = fn(f); \
    }

static StateRect scanRect(FILE *f);
static void batchRect(BatchDrawCall *c, StateRect r, const uint8_t *color);

State *stateNew(const char *path, const StateInput *in) {
    State *state = calloc(1, sizeof(*state));

    state->winW = in->winW;
    state->winH = in->winH;

    FILE *f = fopen(path, "rb");

    for (int i = 0; i < STATE_COLOR_COUNT; ++i) {
        for (int j = 0; j < 4; ++j) {
            fscanf(f, "%" SCNd8, &state->color[i][j]);
        }
    }

    fscanf(f, "%d%d", &state->lvl.w, &state->lvl.h);

    state->hero = scanRect(f);

    StateRectArray wall;
    SCAN_ARRAY(f, wall, scanRect);
    for (size_t i = 0; i < wall.n; ++i) {
        batchRect(&state->bg, wall.arr[i], state->color[STATE_COLOR_WALL]);
    }
    free(wall.arr);

    SCAN_ARRAY(f, state->coin, scanRect);

    fclose(f);

    return state;
}

State *stateDel(State *state) {
    free(state->coin.arr);
    batchDrawCallDel(&state->bg, false);
    free(state);
    return NULL;
}

void stateUpdate(State *state, const StateInput *in) {
    state->winW = in->winW;
    state->winH = in->winH;
}

void stateDraw(State *state) {
    float matrix[4][4];
    rendererViewport(0, 0, state->winW, state->winH);
    Camera2D camera = camera2D((float)state->winW / (float)state->winH);
    camera2DsetMinRect(&camera, 0, 0, state->lvl.w, state->lvl.h);
    camera2Dmatrix(&camera, matrix);
    rendererPipelineWorld(matrix, NULL, NULL, NULL);

    float bgColor[4];
    for (int i = 0; i < 4; ++i) {
        bgColor[i] = state->color[STATE_COLOR_BACKGROUND][i] / 255.0;
    }
    rendererClear(bgColor);
    rendererDrawIndexed(RENDERER_DRAW_MODE_TRIANGLES, state->bg.ni, state->bg.i, state->bg.v);

    batchRect(&state->fg, state->hero, state->color[STATE_COLOR_HERO]);
    for (size_t i = 0; i < state->coin.n; ++i) {
        batchRect(&state->fg, state->coin.arr[i], state->color[STATE_COLOR_COIN]);
    }
    rendererDrawIndexed(RENDERER_DRAW_MODE_TRIANGLES, state->fg.ni, state->fg.i, state->fg.v);
    batchDrawCallClear(&state->fg);
}

static StateRect scanRect(FILE *f) {
    StateRect r;
    fscanf(f, "%d%d%d%d", &r.x, &r.y, &r.w, &r.h);
    return r;
}

static void batchRect(BatchDrawCall *c, StateRect r, const uint8_t *color) {
    batchDrawCallRect2D(c, r.x, r.y, r.w, r.h, color);
}
