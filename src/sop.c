#include "../lib/dragon.h"
#include "state.h"

#define MAX(x,y) ((x)>(y)?(x):(y))

bool stateOpBumpCollision(const State *state, CollPen p) {
    if (p.is && state->hero.vVel <= 0 && p.south > 0) {
        return true;
    }
    return false;
}

bool stateOpGameOver(const State *state) {
    bool outOfBounds = false;
    CollRect r = state->hero.r;
    if (r.x < 0 || r.y < 0 || r.x >= state->lvl.w - 1 || r.y >= state->lvl.h - 1) {
        outOfBounds = true;
    }

    bool allCoinsTaken = true;
    for (size_t i = 0; i < state->coin.n; ++i) {
        if (!state->coin.arr[i].taken) {
            allCoinsTaken = false;
        }
    }

    CollPen p = collBmpRect(state->lvl, state->hero.r);
    bool crushed = (p.south > 0 && p.north > 0) || (p.west > 0 && p.east > 0);

    return outOfBounds || allCoinsTaken || crushed;
}

CollRect stateOpPulsator(const State *state, size_t i) {
    float m = state->physics.pulsatorTable[(state->tick + state->pulsator.arr[i].offset) % state->physics.pulsatorTableSize];
    float w = state->pulsator.arr[i].r.w * m;
    float h = state->pulsator.arr[i].r.h * m;
    float x = state->pulsator.arr[i].r.x + (state->pulsator.arr[i].r.w - w) / 2;
    float y = state->pulsator.arr[i].r.y + (state->pulsator.arr[i].r.h - h) / 2;
    return (CollRect){x, y, w, h};
}

CollPen stateOpColl(const State *state, CollRect r) {
    CollPen p = collBmpRect(state->lvl, r);
    for (size_t i = 0; i < state->pulsator.n; ++i) {
        CollPen p2 = collRect(r, stateOpPulsator(state, i));
        if (p2.is) {
            p.is = true;
            p.south = MAX(p.south, p2.south);
            p.north = MAX(p.north, p2.north);
            p.west = MAX(p.west, p2.west);
            p.east = MAX(p.east, p2.east);
        }
    }
    return p;
}

void stateOpEnvEnergy(const State *state, float *velX, float *velY) {
    CollRect h = state->hero.r;
    *velX = *velY = 0;
    for (size_t i = 0; i < state->pulsator.n; ++i) {
        CollRect r0 = stateOpPulsator(state, i);
        CollPen p = collRect(h, r0);
        if (!stateOpBumpCollision(state, p)) {
            continue;
        }

        ((State*)state)->pulsator.arr[i].offset++;
        CollRect r = stateOpPulsator(state, i);
        ((State*)state)->pulsator.arr[i].offset--;

        *velY += (r.y + r.h) - (r0.y + r0.h);
        float relX = (h.x + h.w / 2 - r0.x) / r0.w;
        *velX += (r.x + relX * r.w) - (h.x + h.w / 2);
    }
}
