#include "../lib/dragon.h"
#include "state.h"

#define MAX(x,y) ((x)>(y)?(x):(y))

static CollPen maxPen(CollPen a, CollPen b);
static void addChangingRectVel(CollRect h, CollRect r0, CollRect r, float *velX, float *velY);
static CollRect multipliedRect(CollRect r, float m);

bool stateOpBumpCollision(const State *state, CollPen p) {
    if (p.is && state->physics.invertedGravity && state->hero.vVel >= 0 && p.north > 0) {
        return true;
    } else if (p.is && !state->physics.invertedGravity && state->hero.vVel <= 0 && p.south > 0) {
        return true;
    }
    return false;
}

StateOpGameOverCause stateOpGameOver(const State *state) {
    CollRect r = state->hero.r;

    if (r.x < 0 || r.y < 0 || r.x >= state->lvl.w - 1 || r.y >= state->lvl.h - 1) {
        return STATE_OP_GAME_OVER_CAUSE_LOST;
    }

    CollPen p = collBmpRect(state->lvl, state->hero.r);
    if ((p.south > 0 && p.north > 0) || (p.west > 0 && p.east > 0)) {
        return STATE_OP_GAME_OVER_CAUSE_LOST;
    }

    bool allCoinsTaken = true;
    for (size_t i = 0; i < state->coin.n; ++i) {
        if (!state->coin.arr[i].taken) {
            allCoinsTaken = false;
        }
    }
    if (allCoinsTaken) {
        return STATE_OP_GAME_OVER_CAUSE_WON;
    }

    return STATE_OP_GAME_OVER_CAUSE_NONE;
}

CollPen stateOpColl(const State *state, CollRect r) {
    CollPen p = collBmpRect(state->lvl, r);
    for (size_t i = 0; i < state->pulsator.n; ++i) {
        p = maxPen(p, collRect(r, stateOpPulsator(state, i)));
    }
    for (size_t i = 0; i < state->shrinker.n; ++i) {
        p = maxPen(p, collRect(r, stateOpShrinker(state, i)));
    }
    for (size_t i = 0; i < state->key.n; ++i) {
        for (size_t j = 0; j < state->key.arr[i].lock.n; ++j) {
            p = maxPen(p, collRect(r, stateOpKeyLock(state, i, j)));
        }
    }
    return p;
}

void stateOpEnvEnergy(const State *state, float *velX, float *velY) {
    *velX = *velY = 0;

    for (size_t i = 0; i < state->pulsator.n; ++i) {
        CollRect r0 = stateOpPulsator(state, i);
        CollPen p = collRect(state->hero.r, r0);

        if (!stateOpBumpCollision(state, p)) {
            continue;
        }

        ((State*)state)->tick++;
        CollRect r = stateOpPulsator(state, i);
        ((State*)state)->tick--;

        addChangingRectVel(state->hero.r, r0, r, velX, velY);
    }

    for (size_t i = 0; i < state->shrinker.n; ++i) {
        if (state->shrinker.arr[i].ticksLeft < 1) {
            continue;
        }

        CollRect r0 = stateOpShrinker(state, i);
        CollPen p = collRect(state->hero.r, r0);

        if (!stateOpBumpCollision(state, p)) {
            continue;
        }

        ((State*)state)->shrinker.arr[i].ticksLeft--;
        CollRect r = stateOpShrinker(state, i);
        ((State*)state)->shrinker.arr[i].ticksLeft++;

        addChangingRectVel(state->hero.r, r0, r, velX, velY);
    }


    for (size_t i = 0; i < state->key.n; ++i) {
        if (state->key.arr[i].ticksLeft < 1) {
            continue;
        }

        for (size_t j = 0; j < state->key.arr[i].lock.n; ++j) {
            CollRect r0 = stateOpKeyLock(state, i, j);
            CollPen p = collRect(state->hero.r, r0);

            if (!stateOpBumpCollision(state, p)) {
                continue;
            }

            ((State*)state)->key.arr[i].ticksLeft--;
            CollRect r = stateOpKeyLock(state, i, j);
            ((State*)state)->key.arr[i].ticksLeft++;

            addChangingRectVel(state->hero.r, r0, r, velX, velY);
        }
    }
}

CollRect stateOpPulsator(const State *state, size_t i) {
    float m = state->physics.pulsatorTable[(state->tick + state->pulsator.arr[i].offset) % state->physics.pulsatorTableSize];
    return multipliedRect(state->pulsator.arr[i].r, m);
}

CollRect stateOpShrinker(const State *state, size_t i) {
    float m = state->shrinker.arr[i].ticksLeft < 0 ? 1 : state->shrinker.arr[i].ticksLeft / (float)state->physics.shrinkingTickCount;
    return multipliedRect(state->shrinker.arr[i].r, m);
}

CollRect stateOpKeyLock(const State *state, size_t i, size_t j) {
    float m = state->key.arr[i].ticksLeft < 0 ? 1 : state->key.arr[i].ticksLeft / (float)state->physics.shrinkingTickCount;
    return multipliedRect(state->key.arr[i].lock.arr[j], m);
}

static CollPen maxPen(CollPen a, CollPen b) {
    if (b.is) {
        a.is = true;
        a.south = MAX(a.south, b.south);
        a.north = MAX(a.north, b.north);
        a.west = MAX(a.west, b.west);
        a.east = MAX(a.east, b.east);
    }
    return a;
}

static void addChangingRectVel(CollRect h, CollRect r0, CollRect r, float *velX, float *velY) {
    *velY += (r.y + r.h) - (r0.y + r0.h);
    float relX = (h.x + h.w / 2 - r0.x) / r0.w;
    *velX += (r.x + relX * r.w) - (h.x + h.w / 2);
}

static CollRect multipliedRect(CollRect r, float m) {
    float w = r.w * m;
    float h = r.h * m;
    float x = r.x + (r.w - w) / 2;
    float y = r.y + (r.h - h) / 2;
    return (CollRect){x, y, w, h};
}
