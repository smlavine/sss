// TODO: get rid of the SCAN_ARRAY macro and all the static functions
// TODO: change the scanning order in accordance to the (incoming?) reordering of the State structure

#include <inttypes.h>
#include <string.h>
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

State s;

void stateNew(const char *path) {
    memset(&s, 0, sizeof(s));
    s.lastTime = 0;
    s.hero.vVel = 0;

    FILE *f = fopen(path, "rb");

    for (int i = 0; i < STATE_COLOR_COUNT; ++i) {
        for (int j = 0; j < 4; ++j) {
            fscanf(f, "%" SCNu8, &s.color[i][j]);
        }
    }

    StatePhysics *p = &s.physics;
    fscanf(f, "%f%f%f%f%f", &p->tickDuration, &p->horVel, &p->jumpVel, &p->gravAcc, &p->termVel);
    fscanf(f, "%d%f", &p->ejectorCooldownTickCount, &p->ejectionVel);
    fscanf(f, "%zu", &p->pulsatorTableSize);
    p->pulsatorTable = malloc(p->pulsatorTableSize * sizeof(*p->pulsatorTable));
    for (size_t i = 0; i < p->pulsatorTableSize; ++i) {
        fscanf(f, "%f", &p->pulsatorTable[i]);
    }
    fscanf(f, "%d", &p->shrinkingTickCount);
    fscanf(f, "%f", &p->antilockLineThickness);

    fscanf(f, "%zu%zu", &s.lvl.w, &s.lvl.h);
    bmpNew(s.lvl.w, s.lvl.h, 1, &s.lvl);
    for (int y = s.lvl.h - 1; y >= 0; --y) {
        for (int x = 0; x < (int)s.lvl.w; ++x) {
            int b;
            fscanf(f, "%d", &b);
            bmpSet(&s.lvl, x, y, 0, b);
        }
    }

    s.hero.r = scanRect(f);

    CollRectArray wall;
    SCAN_ARRAY(f, wall, scanRect);
    for (size_t i = 0; i < wall.n; ++i) {
        batchRect(&s.bg, wall.arr[i], s.color[STATE_COLOR_WALL]);
    }
    free(wall.arr);

    SCAN_ARRAY(f, s.ejector, scanEjector);
    SCAN_ARRAY(f, s.pulsator, scanPulsator);
    SCAN_ARRAY(f, s.shrinker, scanShrinker);
    SCAN_ARRAY(f, s.coin, scanPickable);
    SCAN_ARRAY(f, s.graviton, scanPickable);
    SCAN_ARRAY(f, s.key, scanKey);

    fclose(f);
}

void stateDel(void) {
    bmpDel(&s.lvl, false);
    free(s.physics.pulsatorTable);
    batchDel(&s.bg, false);
    free(s.ejector.arr);
    free(s.pulsator.arr);
    free(s.shrinker.arr);
    free(s.coin.arr);
    free(s.graviton.arr);
    for (size_t i = 0; i < s.key.n; ++i) {
        free(s.key.arr[i].key.arr);
        free(s.key.arr[i].lock.arr);
        free(s.key.arr[i].antilock.arr);
    }
    free(s.key.arr);
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
    for (int i = 0; i < 4; ++i) {
        fscanf(f, "%" SCNu8, &k.antilockColor[i]);
    }
    SCAN_ARRAY(f, k.key, scanPickable);
    k.ticksLeft = -1;
    SCAN_ARRAY(f, k.lock, scanRect);
    SCAN_ARRAY(f, k.antilock, scanRect);
    return k;
}
