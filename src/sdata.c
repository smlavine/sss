#include <stdbool.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>

#include "image.h"
#include "renderer.h"
#include "batch.h"
#include "bitmap.h"
#include "coll.h"
#include "state.h"

#define SCAN_ARRAY(f, item, fn) \
    fscanf(f, "%zu", &item.n); \
    item.arr = malloc(item.n * sizeof(*item.arr)); \
    for (size_t i = 0; i < item.n; ++i) { \
        item.arr[i] = fn(f); \
    }

static Coll2Drect scanRect(FILE *f);
static StatePickable scanPickable(FILE *f);

State *stateNew(const char *path, const StateInput *in) {
    State *state = calloc(1, sizeof(*state));
    state->lastTime = 0;
    state->hero.vVel = 0;


    state->winW = in->winW;
    state->winH = in->winH;

    FILE *f = fopen(path, "rb");

    for (int i = 0; i < STATE_COLOR_COUNT; ++i) {
        for (int j = 0; j < 4; ++j) {
            fscanf(f, "%" SCNd8, &state->color[i][j]);
        }
    }

    StatePhysics *p = &state->physics;
    fscanf(f, "%f%f%f%f%f", &p->tickDuration, &p->horVel, &p->jumpVel, &p->gravAcc, &p->termVel);

    fscanf(f, "%" SCNu32 "%" SCNu32, &state->lvl.w, &state->lvl.h);
    bitmapNew(state->lvl.w, state->lvl.h, 1, &state->lvl);
    for (int y = state->lvl.h - 1; y >= 0; --y) {
        for (int x = 0; x < (int)state->lvl.w; ++x) {
            int b;
            fscanf(f, "%d", &b);
            bitmapSet(&state->lvl, x, y, 0, b);
        }
    }

    fscanf(f, "%f%f%f%f", &state->hero.r.x, &state->hero.r.y, &state->hero.r.w, &state->hero.r.h);

    Coll2DrectArray wall;
    SCAN_ARRAY(f, wall, scanRect);
    for (size_t i = 0; i < wall.n; ++i) {
        batchDrawCallRect2D(&state->bg, wall.arr[i].x, wall.arr[i].y, wall.arr[i].w, wall.arr[i].h, state->color[STATE_COLOR_WALL]);
    }
    free(wall.arr);

    SCAN_ARRAY(f, state->coin, scanPickable);

    fclose(f);

    return state;
}

State *stateDel(State *state) {
    bitmapDel(&state->lvl, false);
    free(state->coin.arr);
    batchDrawCallDel(&state->bg, false);
    free(state);
    return NULL;
}

static Coll2Drect scanRect(FILE *f) {
    Coll2Drect r;
    fscanf(f, "%f%f%f%f", &r.x, &r.y, &r.w, &r.h);
    return r;
}

static StatePickable scanPickable(FILE *f) {
    StatePickable p;
    p.taken = false;
    fscanf(f, "%f%f%f%f", &p.r.x, &p.r.y, &p.r.w, &p.r.h);
    return p;
}
