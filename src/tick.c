#include "../lib/dragon.h"
#include "s.h"

#include <string.h>
#include <time.h>

#define TICK_DURATION           0.02
#define HERO_HOR_VEL            0.20
#define EJECTOR_COOLDOWN_TICKS    10
#define GRAV_ACC               -0.01
#define JUMP_VEL                0.17
#define EJECTION_VEL            0.33
#define TERM_VEL                0.50

static int tick(Saudio audio, bool kUp, bool kLeft, bool kRight, bool kR);
static void sleepSome(double t);

int sTick(Saudio audio,double t,bool kUp,bool kLeft,bool kRight,bool kR){
    audioMusicStream(audio.music);
    while (t - s.tick.lastTime > TICK_DURATION) {
        int r = tick(audio, kUp, kLeft, kRight, kR);
        if (r != 0) {
            return r;
        }
    }
    sleepSome(t - s.tick.lastTime);
    return 0;
}

static int tick(Saudio audio, bool kUp, bool kLeft, bool kRight, bool kR) {
    // Advance time
    ++s.tick.tick;
    s.tick.lastTime += TICK_DURATION;

    // Update hero position
    if (kRight) {
        s.hero.r.x += HERO_HOR_VEL;
    }
    if (kLeft) {
        s.hero.r.x -= HERO_HOR_VEL;
    }
    s.hero.r.y += s.hero.vVel;
    s.hero.r.x += s.hero.envVelX;
    s.hero.r.y += s.hero.envVelY;

    // Update environmental velocity
    sOpEnvEnergy(&s.hero.envVelX, &s.hero.envVelY);

    // Check if ejected; cooldown and trigger the ejectors
    bool ejected = false;
    for (size_t i = 0; i < s.ejector.n; ++i) {
        if (s.ejector.arr[i].cooldown > 0) {
            --s.ejector.arr[i].cooldown;
        }
        if (!s.ejector.arr[i].cooldown) {
            CollPen p = collRect(s.hero.r, s.ejector.arr[i].r);
            if (sOpBumpCollision(p)) {
                ejected = true;
                s.ejector.arr[i].cooldown = EJECTOR_COOLDOWN_TICKS;
            }
        }
    }

    // Shrink and trigger shrinkers
    for (size_t i = 0; i < s.shrinker.n; ++i) {
        if (s.shrinker.arr[i].ticksLeft > 0) {
            --s.shrinker.arr[i].ticksLeft;
            continue;
        }
        if (sOpBumpCollision(collRect(s.hero.r, sOpShrinker(i)))) {
            s.shrinker.arr[i].ticksLeft = S_SHRINKING_TICKS;
        }
    }

    // Fix hero position
    CollPen p = sOpColl(s.hero.r);
    s.hero.r.y += p.south;
    s.hero.r.y -= p.north;
    s.hero.r.x += p.west;
    s.hero.r.x -= p.east;

    // Collect coins
    bool coin = false;
    for (size_t i = 0; i < s.coin.n; ++i) {
        if (!s.coin.arr[i].taken && collRect(s.hero.r, s.coin.arr[i].r).is) {
            s.coin.arr[i].taken = true;
            coin = true;
        }
    }

    // Collect gravitons
    bool graviton = false;
    for (size_t i = 0; i < s.graviton.n; ++i) {
        if (!s.graviton.arr[i].taken
         && collRect(s.hero.r, s.graviton.arr[i].r).is) {
            s.graviton.arr[i].taken = true;
            s.graviton.invertedGravity = !s.graviton.invertedGravity;
            graviton = true;
        }
    }

    // Collect keys, shrink locks, unshrink antilocks
    bool key = false;
    for (size_t i = 0; i < s.key.n; ++i) {
        if (s.key.arr[i].ticksLeft > 0) {
            --s.key.arr[i].ticksLeft;
        }
        if (s.key.arr[i].ticksLeft >= 0) {
            continue;
        }
        bool allKeysTaken = true;
        for (size_t j = 0; j < s.key.arr[i].n; ++j) {
            if (!s.key.arr[i].arr[j].taken
             && collRect(s.hero.r, s.key.arr[i].arr[j].r).is) {
                s.key.arr[i].arr[j].taken = true;
                key = true;
            } else {
                allKeysTaken = false;
            }
        }
        if (allKeysTaken) {
            s.key.arr[i].ticksLeft = S_SHRINKING_TICKS;
        }
    }

    // Update vertical velocity
    bool jumped = kUp && sOpBumpCollision(p);
    if (p.south > 0 && s.hero.vVel < 0) {
        s.hero.vVel = 0;
    }
    if (p.north > 0 && s.hero.vVel > 0) {
        s.hero.vVel = 0;
    }
    if (ejected) {
        s.hero.vVel = s.graviton.invertedGravity ? -EJECTION_VEL:EJECTION_VEL;
        s.hero.envVelX = s.hero.envVelY = 0;
    } else if (jumped) {
        s.hero.vVel = s.graviton.invertedGravity ? -JUMP_VEL : JUMP_VEL;
        s.hero.envVelX = s.hero.envVelY = 0;
    }
    s.hero.vVel += s.graviton.invertedGravity ? -GRAV_ACC : GRAV_ACC;
    if (s.hero.vVel < -TERM_VEL) {
        s.hero.vVel = -TERM_VEL;
    } else if (s.hero.vVel > TERM_VEL) {
        s.hero.vVel = TERM_VEL;
    }

    // Check whether the game is over
    int r = kR ? -1 : sOpGameOver();

    // Play audio
    if (r > 0) {
        audioSoundPlay(audio.win);
    } else if (r < 0) {
        audioSoundPlay(audio.die);
    } else if (ejected) {
        audioSoundPlay(audio.eject);
    } else if (jumped) {
        audioSoundPlay(audio.jump);
    } else if (coin) {
        audioSoundPlay(audio.coin);
    } else if (graviton) {
        audioSoundPlay(audio.graviton);
    } else if (key) {
        audioSoundPlay(audio.key);
    }

    return r;
}

static void sleepSome(double t) {
    time_t seconds = (time_t)t;
    unsigned long long nanoseconds = (t - seconds) * 1000000000;
    struct timespec needed = {seconds, nanoseconds};
    struct timespec remaining;
    nanosleep(&needed, &remaining);
}
