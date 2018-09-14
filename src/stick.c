#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "../lib/dragon.h"
#include "state.h"

static StateGameOverCause tick(State *state, const StateInput *in);
static void sleepSome(double t);

StateGameOverCause stateTick(State *state, const StateInput *in) {
    StateGameOverCause c;
    while (in->time - state->lastTime > state->physics.tickDuration) {
        c = tick(state, in);
        stateAudioPlay(state);
        if (c != STATE_GAME_OVER_CAUSE_NONE) {
            break;
        }
    }
    sleepSome(in->time - state->lastTime);
    return c;
}

static StateGameOverCause tick(State *state, const StateInput *in) {
    // Move events
    for (int i = 0; i < STATE_EVENT_COUNT; ++i) {
        state->event[0][i] = state->event[1][i];
        state->event[1][i] = false;
    }

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
    state->hero.r.x += state->hero.envVelX;
    state->hero.r.y += state->hero.envVelY;

    // Update environmental velocities
    stateOpEnvEnergy(state, &state->hero.envVelX, &state->hero.envVelY);

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

    // Shrink and trigger shrinkers
    for (size_t i = 0; i < state->shrinker.n; ++i) {
        if (state->shrinker.arr[i].ticksLeft > 0) {
            --state->shrinker.arr[i].ticksLeft;
        }
        if (state->shrinker.arr[i].ticksLeft >= 0) {
            continue;
        }
        if (collRect(state->hero.r, stateOpShrinker(state, i)).is) {
            state->shrinker.arr[i].ticksLeft = state->physics.shrinkingTickCount;
        }
    }

    // Fix hero position
    CollPen p = stateOpColl(state, state->hero.r);
    state->hero.r.y += p.south;
    state->hero.r.y -= p.north;
    state->hero.r.x += p.west;
    state->hero.r.x -= p.east;

    // Collect coins
    for (size_t i = 0; i < state->coin.n; ++i) {
        if (!state->coin.arr[i].taken && collRect(state->hero.r, state->coin.arr[i].r).is) {
            state->coin.arr[i].taken = true;
            state->event[1][STATE_EVENT_COIN] = true;
        }
    }

    // Collect gravitons
    for (size_t i = 0; i < state->graviton.n; ++i) {
        if (!state->graviton.arr[i].taken && collRect(state->hero.r, state->graviton.arr[i].r).is) {
            state->graviton.arr[i].taken = true;
            state->physics.invertedGravity = !state->physics.invertedGravity;
            state->event[1][STATE_EVENT_GRAVITON] = true;
        }
    }

    // Collect keys, shrink locks
    for (size_t i = 0; i < state->key.n; ++i) {
        if (state->key.arr[i].ticksLeft > 0) {
            --state->key.arr[i].ticksLeft;
        }
        if (state->key.arr[i].ticksLeft >= 0) {
            continue;
        }
        bool allKeysTaken = true;
        for (size_t j = 0; j < state->key.arr[i].key.n; ++j) {
            if (!state->key.arr[i].key.arr[j].taken) {
                if (collRect(state->hero.r, state->key.arr[i].key.arr[j].r).is) {
                    state->key.arr[i].key.arr[j].taken = true;
                    state->event[1][STATE_EVENT_KEY] = true;
                } else {
                    allKeysTaken = false;
                }
            }
        }
        if (allKeysTaken) {
            state->key.arr[i].ticksLeft = state->physics.shrinkingTickCount;
        }
    }

    // Update vertical velocity
    bool jump = false;
    if (in->keyUp && stateOpBumpCollision(state, p)) {
        jump = true;
    }
    if (p.south > 0 && state->hero.vVel < 0) {
        state->hero.vVel = 0;
    }
    if (p.north > 0 && state->hero.vVel > 0) {
        state->hero.vVel = 0;
    }
    if (ejected) {
        state->hero.vVel = state->physics.invertedGravity ? -state->physics.ejectionVel : state->physics.ejectionVel;
        state->hero.envVelX = state->hero.envVelY = 0;
        state->event[1][STATE_EVENT_EJECT] = true;
    } else if (jump) {
        state->hero.vVel = state->physics.invertedGravity ? -state->physics.jumpVel : state->physics.jumpVel;
        state->hero.envVelX = state->hero.envVelY = 0;
        state->event[1][STATE_EVENT_JUMP] = true;
    }
    state->hero.vVel += state->physics.invertedGravity ? -state->physics.gravAcc : state->physics.gravAcc;
    if (state->hero.vVel < -state->physics.termVel) {
        state->hero.vVel = -state->physics.termVel;
    } else if (state->hero.vVel > state->physics.termVel) {
        state->hero.vVel = state->physics.termVel;
    }

    // Check if it's over
    StateGameOverCause c =  in->keyR ? STATE_GAME_OVER_CAUSE_RESTART : stateOpGameOver(state);
    if (c == STATE_GAME_OVER_CAUSE_WON)
        state->event[1][STATE_EVENT_WIN] = true;
    else if (c != STATE_GAME_OVER_CAUSE_NONE)
        state->event[1][STATE_EVENT_DIE] = true;
    return c;
}

static void sleepSome(double t) {
    time_t seconds = (time_t)t;
    unsigned long long nanoseconds = (t - seconds) * 1000000000;
    struct timespec needed = {seconds, nanoseconds};
    struct timespec remaining;
    nanosleep(&needed, &remaining);
}
