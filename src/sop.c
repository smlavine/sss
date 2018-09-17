#include "../lib/dragon.h"
#include "state.h"

#define MAX(x,y) ((x)>(y)?(x):(y))

static CollPen maxPen(CollPen a, CollPen b);
static void addChangingRectVel(CollRect h, CollRect r0, CollRect r, float *velX, float *velY);
static CollRect multipliedRect(CollRect r, float m);

bool stateOpBumpCollision(CollPen p) {
    if (p.is && s.physics.invertedGravity && s.hero.vVel >= 0 && p.north > 0) {
        return true;
    } else if (p.is && !s.physics.invertedGravity && s.hero.vVel <= 0 && p.south > 0) {
        return true;
    }
    return false;
}

StateGameOverCause stateOpGameOver(void) {
    CollRect r = s.hero.r;

    if (r.x < 0 || r.y < 0 || r.x >= s.lvl.w - 1 || r.y >= s.lvl.h - 1) {
        return STATE_GAME_OVER_CAUSE_LOST;
    }

    CollPen p = collBmpRect(s.lvl, s.hero.r);
    if ((p.south > 0 && p.north > 0) || (p.west > 0 && p.east > 0)) {
        return STATE_GAME_OVER_CAUSE_LOST;
    }

    bool allCoinsTaken = true;
    for (size_t i = 0; i < s.coin.n; ++i) {
        if (!s.coin.arr[i].taken) {
            allCoinsTaken = false;
        }
    }
    if (allCoinsTaken) {
        return STATE_GAME_OVER_CAUSE_WON;
    }

    return STATE_GAME_OVER_CAUSE_NONE;
}

CollPen stateOpColl(CollRect r) {
    CollPen p = collBmpRect(s.lvl, r);
    for (size_t i = 0; i < s.pulsator.n; ++i) {
        p = maxPen(p, collRect(r, stateOpPulsator(i)));
    }
    for (size_t i = 0; i < s.shrinker.n; ++i) {
        p = maxPen(p, collRect(r, stateOpShrinker(i)));
    }
    for (size_t i = 0; i < s.key.n; ++i) {
        for (size_t j = 0; j < s.key.arr[i].lock.n; ++j) {
            p = maxPen(p, collRect(r, stateOpKeyLock(i, j)));
        }
        for (size_t j = 0; j < s.key.arr[i].antilock.n; ++j) {
            p = maxPen(p, collRect(r, stateOpKeyAntilock(i, j)));
        }
    }
    return p;
}

void stateOpEnvEnergy(float *velX, float *velY) {
    *velX = *velY = 0;

    for (size_t i = 0; i < s.pulsator.n; ++i) {
        CollRect r0 = stateOpPulsator(i);
        CollPen p = collRect(s.hero.r, r0);

        if (!stateOpBumpCollision(p)) {
            continue;
        }

        s.tick++;
        CollRect r = stateOpPulsator(i);
        s.tick--;

        addChangingRectVel(s.hero.r, r0, r, velX, velY);
    }

    for (size_t i = 0; i < s.shrinker.n; ++i) {
        if (s.shrinker.arr[i].ticksLeft < 1) {
            continue;
        }

        CollRect r0 = stateOpShrinker(i);
        CollPen p = collRect(s.hero.r, r0);

        if (!stateOpBumpCollision(p)) {
            continue;
        }

        s.shrinker.arr[i].ticksLeft--;
        CollRect r = stateOpShrinker(i);
        s.shrinker.arr[i].ticksLeft++;

        addChangingRectVel(s.hero.r, r0, r, velX, velY);
    }


    for (size_t i = 0; i < s.key.n; ++i) {
        if (s.key.arr[i].ticksLeft < 1) {
            continue;
        }

        for (size_t j = 0; j < s.key.arr[i].lock.n; ++j) {
            CollRect r0 = stateOpKeyLock(i, j);
            CollPen p = collRect(s.hero.r, r0);

            if (!stateOpBumpCollision(p)) {
                continue;
            }

            s.key.arr[i].ticksLeft--;
            CollRect r = stateOpKeyLock(i, j);
            s.key.arr[i].ticksLeft++;

            addChangingRectVel(s.hero.r, r0, r, velX, velY);
        }

        for (size_t j = 0; j < s.key.arr[i].antilock.n; ++j) {
            CollRect r0 = stateOpKeyAntilock(i, j);
            CollPen p = collRect(s.hero.r, r0);

            if (!stateOpBumpCollision(p)) {
                continue;
            }

            s.key.arr[i].ticksLeft--;
            CollRect r = stateOpKeyAntilock(i, j);
            s.key.arr[i].ticksLeft++;

            addChangingRectVel(s.hero.r, r0, r, velX, velY);
        }
    }
}

CollRect stateOpPulsator(size_t i) {
    float m = s.physics.pulsatorTable[(s.tick + s.pulsator.arr[i].offset) % s.physics.pulsatorTableSize];
    return multipliedRect(s.pulsator.arr[i].r, m);
}

CollRect stateOpShrinker(size_t i) {
    float m = s.shrinker.arr[i].ticksLeft < 0 ? 1 : s.shrinker.arr[i].ticksLeft / (float)s.physics.shrinkingTickCount;
    return multipliedRect(s.shrinker.arr[i].r, m);
}

CollRect stateOpKeyLock(size_t i, size_t j) {
    float m = s.key.arr[i].ticksLeft < 0 ? 1 : s.key.arr[i].ticksLeft / (float)s.physics.shrinkingTickCount;
    return multipliedRect(s.key.arr[i].lock.arr[j], m);
}

CollRect stateOpKeyAntilock(size_t i, size_t j) {
    float m = s.key.arr[i].ticksLeft < 0 ? 0 : 1 - s.key.arr[i].ticksLeft / (float)s.physics.shrinkingTickCount;
    return multipliedRect(s.key.arr[i].antilock.arr[j], m);
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
