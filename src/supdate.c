#include <stdlib.h>
#include <time.h>

#include "../lib/dragon.h"
#include "state.h"

static void update(State *state, const StateInput *in);
static bool stateOpBumpCollision(const State *s, Coll2Dpen p);
static void sleepSome(double t);

void stateUpdate(State *state, const StateInput *in) {
    while (in->time - state->lastTime > state->physics.tickDuration) {
        update(state, in);
    }
    sleepSome(in->time - state->lastTime);
}

static void update(State *state, const StateInput *in) {
    state->winW = in->winW;
    state->winH = in->winH;

    state->lastTime += state->physics.tickDuration;
    ++state->tick;

    if (in->keyRight) {
        state->hero.r.x += state->physics.horVel;
    }
    if (in->keyLeft) {
        state->hero.r.x -= state->physics.horVel;
    }
    state->hero.r.y += state->hero.vVel;

    Coll2Dpen p = coll2DbitmapRect(state->lvl, state->hero.r);
    state->hero.r.y += p.south;
    state->hero.r.y -= p.north;
    state->hero.r.x += p.west;
    state->hero.r.x -= p.east;

    for (size_t i = 0; i < state->coin.n; ++i) {
        if (!state->coin.arr[i].taken && coll2Drect(state->hero.r, state->coin.arr[i].r).is) {
            state->coin.arr[i].taken = true;
        }
    }

    bool jump = false;
    if (in->keyUp && stateOpBumpCollision(state,p)) {
        jump = true;
    }
    if (p.south > 0 && state->hero.vVel < 0) {
        state->hero.vVel = 0;
    }
    if (p.north > 0 && state->hero.vVel > 0) {
        state->hero.vVel = 0;
    }
    if (jump) {
        state->hero.vVel = state->physics.jumpVel;
    }
    state->hero.vVel += state->physics.gravAcc;
    if (state->hero.vVel < -state->physics.termVel) {
        state->hero.vVel = -state->physics.termVel;
    } else if (state->hero.vVel > state->physics.termVel) {
        state->hero.vVel = state->physics.termVel;
    }

    bool allCoinsTaken = true;
    for (size_t i = 0; i < state->coin.n; ++i) {
        if (!state->coin.arr[i].taken) {
            allCoinsTaken = false;
        }
    }

    if (allCoinsTaken) {
        exit(EXIT_SUCCESS);
    }
    if ((p.south > 0 && p.north > 0) || (p.west > 0 && p.east > 0)) {
        exit(EXIT_FAILURE);
    }
}

static bool stateOpBumpCollision(const State *s, Coll2Dpen p) {
    if (p.is && s->hero.vVel <= 0 && p.south > 0) {
        return true;
    }
    return false;
}

static void sleepSome(double t) {
    time_t seconds = (time_t)t;
    unsigned long long nanoseconds = (t - seconds) * 1000000000;
    struct timespec needed = {seconds, nanoseconds};
    struct timespec remaining;
    nanosleep(&needed, &remaining);
}
