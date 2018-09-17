#include <string.h>
#include <time.h>

#include "../lib/dragon.h"
#include "state.h"

static StateGameOverCause tick(const StateInput *in);
static void sleepSome(double t);

StateGameOverCause stateTick(const StateInput *in) {
    StateGameOverCause c;
    while (in->time - s.lastTime > s.physics.tickDuration) {
        c = tick(in);
        stateAudioPlay();
        if (c != STATE_GAME_OVER_CAUSE_NONE) {
            break;
        }
    }
    sleepSome(in->time - s.lastTime);
    return c;
}

static StateGameOverCause tick(const StateInput *in) {
    // Move events
    memset(s.event, 0, sizeof(s.event));

    // Update state with data received from input
    s.winW = in->winW;
    s.winH = in->winH;

    // Advance time
    s.lastTime += s.physics.tickDuration;
    ++s.tick;

    // Update hero position
    if (in->keyRight) {
        s.hero.r.x += s.physics.horVel;
    }
    if (in->keyLeft) {
        s.hero.r.x -= s.physics.horVel;
    }
    s.hero.r.y += s.hero.vVel;
    s.hero.r.x += s.hero.envVelX;
    s.hero.r.y += s.hero.envVelY;

    // Update environmental velocities
    stateOpEnvEnergy(&s.hero.envVelX, &s.hero.envVelY);

    // Check if ejected; cooldown and trigger the ejectors
    bool ejected = false;
    for (size_t i = 0; i < s.ejector.n; ++i) {
        if (s.ejector.arr[i].cooldown > 0) {
            --s.ejector.arr[i].cooldown;
        }
        if (!s.ejector.arr[i].cooldown) {
            CollPen p = collRect(s.hero.r, s.ejector.arr[i].r);
            if (stateOpBumpCollision(p)) {
                ejected = true;
                s.ejector.arr[i].cooldown = s.physics.ejectorCooldownTickCount;
            }
        }
    }

    // Shrink and trigger shrinkers
    for (size_t i = 0; i < s.shrinker.n; ++i) {
        if (s.shrinker.arr[i].ticksLeft > 0) {
            --s.shrinker.arr[i].ticksLeft;
        }
        if (s.shrinker.arr[i].ticksLeft >= 0) {
            continue;
        }
        if (collRect(s.hero.r, stateOpShrinker(i)).is) {
            s.shrinker.arr[i].ticksLeft = s.physics.shrinkingTickCount;
        }
    }

    // Fix hero position
    CollPen p = stateOpColl(s.hero.r);
    s.hero.r.y += p.south;
    s.hero.r.y -= p.north;
    s.hero.r.x += p.west;
    s.hero.r.x -= p.east;

    // Collect coins
    for (size_t i = 0; i < s.coin.n; ++i) {
        if (!s.coin.arr[i].taken && collRect(s.hero.r, s.coin.arr[i].r).is) {
            s.coin.arr[i].taken = true;
            s.event[STATE_EVENT_COIN] = true;
        }
    }

    // Collect gravitons
    for (size_t i = 0; i < s.graviton.n; ++i) {
        if (!s.graviton.arr[i].taken && collRect(s.hero.r, s.graviton.arr[i].r).is) {
            s.graviton.arr[i].taken = true;
            s.physics.invertedGravity = !s.physics.invertedGravity;
            s.event[STATE_EVENT_GRAVITON] = true;
        }
    }

    // Collect keys, shrink locks, unshrink antilocks
    for (size_t i = 0; i < s.key.n; ++i) {
        if (s.key.arr[i].ticksLeft > 0) {
            --s.key.arr[i].ticksLeft;
        }
        if (s.key.arr[i].ticksLeft >= 0) {
            continue;
        }
        bool allKeysTaken = true;
        for (size_t j = 0; j < s.key.arr[i].key.n; ++j) {
            if (!s.key.arr[i].key.arr[j].taken) {
                if (collRect(s.hero.r, s.key.arr[i].key.arr[j].r).is) {
                    s.key.arr[i].key.arr[j].taken = true;
                    s.event[STATE_EVENT_KEY] = true;
                } else {
                    allKeysTaken = false;
                }
            }
        }
        if (allKeysTaken) {
            s.key.arr[i].ticksLeft = s.physics.shrinkingTickCount;
        }
    }

    // Update vertical velocity
    bool jump = false;
    if (in->keyUp && stateOpBumpCollision(p)) {
        jump = true;
    }
    if (p.south > 0 && s.hero.vVel < 0) {
        s.hero.vVel = 0;
    }
    if (p.north > 0 && s.hero.vVel > 0) {
        s.hero.vVel = 0;
    }
    if (ejected) {
        s.hero.vVel = s.physics.invertedGravity ? -s.physics.ejectionVel : s.physics.ejectionVel;
        s.hero.envVelX = s.hero.envVelY = 0;
        s.event[STATE_EVENT_EJECT] = true;
    } else if (jump) {
        s.hero.vVel = s.physics.invertedGravity ? -s.physics.jumpVel : s.physics.jumpVel;
        s.hero.envVelX = s.hero.envVelY = 0;
        s.event[STATE_EVENT_JUMP] = true;
    }
    s.hero.vVel += s.physics.invertedGravity ? -s.physics.gravAcc : s.physics.gravAcc;
    if (s.hero.vVel < -s.physics.termVel) {
        s.hero.vVel = -s.physics.termVel;
    } else if (s.hero.vVel > s.physics.termVel) {
        s.hero.vVel = s.physics.termVel;
    }

    // Check if it's over
    StateGameOverCause c =  in->keyR ? STATE_GAME_OVER_CAUSE_RESTART : stateOpGameOver();
    if (c == STATE_GAME_OVER_CAUSE_WON)
        s.event[STATE_EVENT_WIN] = true;
    else if (c != STATE_GAME_OVER_CAUSE_NONE)
        s.event[STATE_EVENT_DIE] = true;
    return c;
}

static void sleepSome(double t) {
    time_t seconds = (time_t)t;
    unsigned long long nanoseconds = (t - seconds) * 1000000000;
    struct timespec needed = {seconds, nanoseconds};
    struct timespec remaining;
    nanosleep(&needed, &remaining);
}
