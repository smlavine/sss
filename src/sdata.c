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
static StateEjector scanEjector(FILE *f);
static StatePulsator scanPulsator(FILE *f);
static StateShrinker scanShrinker(FILE *f);
static StatePickable scanPickable(FILE *f);
static StateKey scanKey(FILE *f);

State *stateNew(const char *path, const StateInput *in) {
    State *state = calloc(1, sizeof(*state));
    state->lastTime = 0;
    state->hero.vVel = 0;

    state->winW = in->winW;
    state->winH = in->winH;

    FILE *f = fopen(path, "rb");

    for (int i = 0; i < STATE_COLOR_COUNT; ++i) {
        for (int j = 0; j < 4; ++j) {
            fscanf(f, "%" SCNu8, &state->color[i][j]);
        }
    }

    StatePhysics *p = &state->physics;
    fscanf(f, "%f%f%f%f%f", &p->tickDuration, &p->horVel, &p->jumpVel, &p->gravAcc, &p->termVel);
    fscanf(f, "%d%f", &p->ejectorCooldownTickCount, &p->ejectionVel);
    fscanf(f, "%zu", &p->pulsatorTableSize);
    p->pulsatorTable = malloc(p->pulsatorTableSize * sizeof(*p->pulsatorTable));
    for (size_t i = 0; i < p->pulsatorTableSize; ++i) {
        fscanf(f, "%f", &p->pulsatorTable[i]);
    }
    fscanf(f, "%d", &p->shrinkingTickCount);
    fscanf(f, "%f", &p->jumpitonVel);

    fscanf(f, "%" SCNu32 "%" SCNu32, &state->lvl.w, &state->lvl.h);
    bmpNew(state->lvl.w, state->lvl.h, 1, &state->lvl);
    for (int y = state->lvl.h - 1; y >= 0; --y) {
        for (int x = 0; x < (int)state->lvl.w; ++x) {
            int b;
            fscanf(f, "%d", &b);
            bmpSet(&state->lvl, x, y, 0, b);
        }
    }

    state->hero.r = scanRect(f);

    CollRectArray wall;
    SCAN_ARRAY(f, wall, scanRect);
    for (size_t i = 0; i < wall.n; ++i) {
        batchCallRect(&state->bg, wall.arr[i], state->color[STATE_COLOR_WALL]);
    }
    free(wall.arr);

    SCAN_ARRAY(f, state->ejector, scanEjector);
    SCAN_ARRAY(f, state->pulsator, scanPulsator);
    SCAN_ARRAY(f, state->shrinker, scanShrinker);
    SCAN_ARRAY(f, state->coin, scanPickable);
    SCAN_ARRAY(f, state->graviton, scanPickable);
    SCAN_ARRAY(f, state->jumpiton, scanPickable);
    SCAN_ARRAY(f, state->key, scanKey);

    fclose(f);

    return state;
}

State *stateDel(State *state) {
    bmpDel(&state->lvl, false);
    free(state->physics.pulsatorTable);
    batchCallDel(&state->bg, false);
    free(state->ejector.arr);
    free(state->pulsator.arr);
    free(state->shrinker.arr);
    free(state->coin.arr);
    free(state->graviton.arr);
    for (size_t i = 0; i < state->key.n; ++i) {
        free(state->key.arr[i].key.arr);
        free(state->key.arr[i].lock.arr);
    }
    free(state->key.arr);
    free(state);
    return NULL;
}

static CollRect scanRect(FILE *f) {
    CollRect r;
    fscanf(f, "%f%f%f%f", &r.x, &r.y, &r.w, &r.h);
    return r;
}

static StateEjector scanEjector(FILE *f) {
    StateEjector e;
    e.cooldown = 0;
    e.r = scanRect(f);
    return e;
}

static StatePulsator scanPulsator(FILE *f) {
    StatePulsator p;
    fscanf(f, "%d", &p.offset);
    p.r = scanRect(f);
    return p;
}

static StateShrinker scanShrinker(FILE *f) {
    StateShrinker s;
    s.ticksLeft = -1;
    s.r = scanRect(f);
    return s;
}

static StatePickable scanPickable(FILE *f) {
    StatePickable p;
    p.taken = false;
    p.r = scanRect(f);
    return p;
}

static StateKey scanKey(FILE *f) {
    StateKey k;
    for (int i = 0; i < 4; ++i) {
        fscanf(f, "%" SCNu8, &k.keyColor[i]);
    }
    for (int i = 0; i < 4; ++i) {
        fscanf(f, "%" SCNu8, &k.lockColor[i]);
    }
    SCAN_ARRAY(f, k.key, scanPickable);
    k.ticksLeft = -1;
    SCAN_ARRAY(f, k.lock, scanRect);
    return k;
}
