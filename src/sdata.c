#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>

#include "../lib/dragon.h"
#include "state.h"

#define SCAN_ARRAY(f, item, fn) \
    fscanf(f, "%zu", &item.n); \
    item.arr = malloc(item.n * sizeof(*item.arr)); \
    for (size_t i = 0; i < item.n; ++i) { \
        item.arr[i] = fn(f); \
    }

static CollRect scanRect(FILE *f);
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
    bmpNew(state->lvl.w, state->lvl.h, 1, &state->lvl);
    for (int y = state->lvl.h - 1; y >= 0; --y) {
        for (int x = 0; x < (int)state->lvl.w; ++x) {
            int b;
            fscanf(f, "%d", &b);
            bmpSet(&state->lvl, x, y, 0, b);
        }
    }

    fscanf(f, "%f%f%f%f", &state->hero.r.x, &state->hero.r.y, &state->hero.r.w, &state->hero.r.h);

    CollRectArray wall;
    SCAN_ARRAY(f, wall, scanRect);
    for (size_t i = 0; i < wall.n; ++i) {
        batchCallRect(&state->bg, wall.arr[i], state->color[STATE_COLOR_WALL]);
    }
    free(wall.arr);

    SCAN_ARRAY(f, state->coin, scanPickable);

    fclose(f);

    return state;
}

State *stateDel(State *state) {
    bmpDel(&state->lvl, false);
    free(state->coin.arr);
    batchCallDel(&state->bg, false);
    free(state);
    return NULL;
}

static CollRect scanRect(FILE *f) {
    CollRect r;
    fscanf(f, "%f%f%f%f", &r.x, &r.y, &r.w, &r.h);
    return r;
}

static StatePickable scanPickable(FILE *f) {
    StatePickable p;
    p.taken = false;
    p.r = scanRect(f);
    return p;
}
