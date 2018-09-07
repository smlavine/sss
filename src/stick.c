#include <stdlib.h>
#include <stdlib.h>
#include <time.h>

#include "../lib/dragon.h"
#include "state.h"

static void tick(State *state, const StateInput *in);
static void sleepSome(double t);

void stateTick(State *state, const StateInput *in) {
    while (in->time - state->lastTime > state->physics.tickDuration) {
        tick(state, in);
    }
    sleepSome(in->time - state->lastTime);
}

static void tick(State *state, const StateInput *in) {
    // Update state with data received from input
    state->winW = in->winW;
    state->winH = in->winH;

    // Advance time
    state->lastTime += state->physics.tickDuration;
    ++state->tick;

    // Update hero position
    if (in->keyRight) {
        state->hero.r.x += state->physics.horVel;
    }
    if (in->keyLeft) {
        state->hero.r.x -= state->physics.horVel;
    }
    state->hero.r.y += state->hero.vVel;

    // Check if ejected; cooldown and trigger the ejectors
    bool ejected = false;
    for (size_t i = 0; i < state->ejector.n; ++i) {
        if (state->ejector.arr[i].cooldown > 0) {
            --state->ejector.arr[i].cooldown;
        }
        if (!state->ejector.arr[i].cooldown) {
            CollPen p = collRect(state->hero.r, state->ejector.arr[i].r);
            if (stateOpBumpCollision(state, p)) {
                ejected = true;
                state->ejector.arr[i].cooldown = state->physics.ejectorCooldownTickCount;
            }
        }
    }

    // Fix hero position
    CollPen p = collBmpRect(state->lvl, state->hero.r);
    state->hero.r.y += p.south;
    state->hero.r.y -= p.north;
    state->hero.r.x += p.west;
    state->hero.r.x -= p.east;

    // Collect coins
    for (size_t i = 0; i < state->coin.n; ++i) {
        if (!state->coin.arr[i].taken && collRect(state->hero.r, state->coin.arr[i].r).is) {
            state->coin.arr[i].taken = true;
        }
    }

    // Update vertical velocity
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
    if (ejected) {
        state->hero.vVel = state->physics.ejectionVel;
    } else if (jump) {
        state->hero.vVel = state->physics.jumpVel;
    }
    state->hero.vVel += state->physics.gravAcc;
    if (state->hero.vVel < -state->physics.termVel) {
        state->hero.vVel = -state->physics.termVel;
    } else if (state->hero.vVel > state->physics.termVel) {
        state->hero.vVel = state->physics.termVel;
    }

    // Check if it's over
    if (stateOpGameOver(state)) {
        exit(EXIT_SUCCESS);
    }
}

static void sleepSome(double t) {
    time_t seconds = (time_t)t;
    unsigned long long nanoseconds = (t - seconds) * 1000000000;
    struct timespec needed = {seconds, nanoseconds};
    struct timespec remaining;
    nanosleep(&needed, &remaining);
}
