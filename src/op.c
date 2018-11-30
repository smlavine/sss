#include "s.h"

#define LEN(x) (sizeof(x)/sizeof(x[0]))
#define MAX(x,y) ((x)>(y)?(x):(y))

#define PULSATOR_TABLE (const float[]) { \
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, \
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, \
    0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0,    \
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, \
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, \
    0.9, 0.8, 0.7, 0.6, 0.5, 0.4, 0.3, 0.2, 0.1, 0.0     }
#define MIN_SWITCH_CLEARANCE 0.01

static CollRect multipliedRect(CollRect r, float m);
static CollPen maxPen(CollPen a, CollPen b);
static void addRectV(CollRect h,CollRect r0,CollRect r,float *vX,float *vY);

CollRect sOpPulsator(size_t i) {
    size_t ti = (s.tick.tick + s.pulsator.arr[i].offset)%LEN(PULSATOR_TABLE);
    return multipliedRect(s.pulsator.arr[i].r, PULSATOR_TABLE[ti]);
}

CollRect sOpShrinker(size_t i) {
    float m = s.shrinker.arr[i].ticksLeft < 0 ? 1
            : s.shrinker.arr[i].ticksLeft / (float)S_SHRINKING_TICKS;
    return multipliedRect(s.shrinker.arr[i].r, m);
}

CollRect sOpKeyLock(size_t i, size_t j) {
    float m = s.key.arr[i].ticksLeft < 0 ? 1
            : s.key.arr[i].ticksLeft / (float)S_SHRINKING_TICKS;
    return multipliedRect(s.key.arr[i].lock.arr[j], m);
}

CollRect sOpKeyAntilock(size_t i, size_t j) {
    float m = s.key.arr[i].ticksLeft < 0 ? 0
            : 1 - s.key.arr[i].ticksLeft / (float)S_SHRINKING_TICKS;
    return multipliedRect(s.key.arr[i].antilock.arr[j], m);
}

CollRect sOpPlat(size_t i) {
    size_t j = s.tick.tick % s.plat.arr[i].n;
    float x = s.plat.arr[i].arr[j].x;
    float y = s.plat.arr[i].arr[j].y;
    return (CollRect){x, y, s.plat.arr[i].w, s.plat.arr[i].h};
}

CollPen sOpColl(CollRect r) {
    CollPen p = collBmpRect(s.lvl, r);
    for (size_t i = 0; i < s.pulsator.n; ++i) {
        p = maxPen(p, collRect(r, sOpPulsator(i)));
    }
    for (size_t i = 0; i < s.shrinker.n; ++i) {
        p = maxPen(p, collRect(r, sOpShrinker(i)));
    }
    for (size_t i = 0; i < s.key.n; ++i) {
        for (size_t j = 0; j < s.key.arr[i].lock.n; ++j) {
            p = maxPen(p, collRect(r, sOpKeyLock(i, j)));
        }
        for (size_t j = 0; j < s.key.arr[i].antilock.n; ++j) {
            p = maxPen(p, collRect(r, sOpKeyAntilock(i, j)));
        }
    }
    for (size_t i = 0; i < s.plat.n; ++i) {
        p = maxPen(p, collRect(r, sOpPlat(i)));
    }
    for (size_t i = 0; i < s.hero.n; ++i) {
        if (i == s.hero.i) {
            continue;
        }
        p = maxPen(p, collRect(r, s.hero.arr[i].r));
    }
    return p;
}

int sOpGameOver(void) {
    CollRect r = s.hero.arr[s.hero.i].r;
    if (r.x < 0 || r.y < 0 || r.x >= s.lvl.w - r.w || r.y >= s.lvl.h - r.h) {
        return -1; // out of bounds
    }

    CollPen p = collBmpRect(s.lvl, s.hero.arr[s.hero.i].r);
    if ((p.south > 0 && p.north > 0) || (p.west > 0 && p.east > 0)) {
        return -1; // crushed
    }

    bool allCoinsTaken = true;
    for (size_t i = 0; i < s.coin.n; ++i) {
        if (!s.coin.arr[i].taken) {
            allCoinsTaken = false;
        }
    }
    if (allCoinsTaken) {
        return 1; // took all the coins and won
    }

    return 0; // not over
}

bool sOpBumpCollision(CollPen p) {
    if ((p.is &&  s.graviton.invertedGravity && s.hero.vVel>=0 && p.north>0)
      ||(p.is && !s.graviton.invertedGravity && s.hero.vVel<=0 && p.south>0)){
        return true;
    }
    return false;
}

void sOpEnvEnergy(float *velX, float *velY) {
    *velX = *velY = 0;

    for (size_t i = 0; i < s.pulsator.n; ++i) {
        CollRect r0 = sOpPulsator(i);
        if (!sOpBumpCollision(collRect(s.hero.arr[s.hero.i].r, r0))) {
            continue;
        }
        s.tick.tick++;
        addRectV(s.hero.arr[s.hero.i].r, r0, sOpPulsator(i), velX, velY);
        s.tick.tick--;
    }

    for (size_t i = 0; i < s.shrinker.n; ++i) {
        CollRect r0 = sOpShrinker(i);
        if (s.shrinker.arr[i].ticksLeft < 1
         || !sOpBumpCollision(collRect(s.hero.arr[s.hero.i].r, r0))) {
            continue;
        }
        s.shrinker.arr[i].ticksLeft--;
        addRectV(s.hero.arr[s.hero.i].r, r0, sOpShrinker(i), velX, velY);
        s.shrinker.arr[i].ticksLeft++;
    }

    for (size_t i = 0; i < s.key.n; ++i) {
        if (s.key.arr[i].ticksLeft < 1) {
            continue;
        }

        for (size_t j = 0; j < s.key.arr[i].lock.n; ++j) {
            CollRect r0 = sOpKeyLock(i, j);
            if (!sOpBumpCollision(collRect(s.hero.arr[s.hero.i].r, r0))) {
                continue;
            }
            s.key.arr[i].ticksLeft--;
            addRectV(s.hero.arr[s.hero.i].r, r0, sOpKeyLock(i, j), velX, velY);
            s.key.arr[i].ticksLeft++;

        }

        for (size_t j = 0; j < s.key.arr[i].antilock.n; ++j) {
            CollRect r0 = sOpKeyAntilock(i, j);
            if (!sOpBumpCollision(collRect(s.hero.arr[s.hero.i].r, r0))) {
                continue;
            }
            s.key.arr[i].ticksLeft--;
            addRectV(s.hero.arr[s.hero.i].r, r0, sOpKeyAntilock(i, j), velX,velY);
            s.key.arr[i].ticksLeft++;
        }
    }

    for (size_t i = 0; i < s.plat.n; ++i) {
        CollRect r0 = sOpPlat(i);
        if (!sOpBumpCollision(collRect(s.hero.arr[s.hero.i].r, r0))) {
            continue;
        }
        s.tick.tick++;
        addRectV(s.hero.arr[s.hero.i].r, r0, sOpPlat(i), velX, velY);
        s.tick.tick--;
    }

}

int sOpSwitch(CollPen p, bool kShft, bool kTab) {
    CollRect r = s.hero.arr[s.hero.i].r;
    if (s.hero.n<2||!kTab||!sOpBumpCollision(p)||collBmpRect(s.tab,r).is) {
        return -1;
    }

    int dir = kShft ? -1 : 1;
    for (int i = 1; i < (int)s.hero.n; ++i) {
        int index = (int)s.hero.i + i * dir;
        index = index < 0 ? (int)s.hero.n + index : index % (int)s.hero.n;
        CollRect R = s.hero.arr[index].r;

        bool ok = true;
        for (int j = 0; j < (int)s.hero.n; ++j) {
            if (j == index) {
                continue;
            }

            r = s.hero.arr[j].r;
            bool outside = r.x + r.w <= R.x || r.x >= R.x + R.w;
            bool below=r.y+r.h/2>=R.y+R.h/2&&r.y<=R.y+R.h+MIN_SWITCH_CLEARANCE;
            bool above=r.y+r.h/2<=R.y+R.h/2&&r.y+r.h>=R.y-MIN_SWITCH_CLEARANCE;

            if (!s.hero.arr[index].invertedGravity && (!outside && below)) {
                ok = false;
                break;
            } else if (s.hero.arr[index].invertedGravity&&(!outside&&above)) {
                ok = false;
                break;
            }

        }

        if (ok) {
            return index;
        }
    }

    return -1;
}

static CollRect multipliedRect(CollRect r, float m) {
    float w = r.w * m;
    float h = r.h * m;
    float x = r.x + (r.w - w) / 2;
    float y = r.y + (r.h - h) / 2;
    return (CollRect){x, y, w, h};
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

static void addRectV(CollRect h,CollRect r0,CollRect r,float *vX,float *vY) {
    *vY += (r.y + r.h) - (r0.y + r0.h);
    float relX = (h.x + h.w / 2 - r0.x) / r0.w;
    *vX += (r.x + relX * r.w) - (h.x + h.w / 2);
}
