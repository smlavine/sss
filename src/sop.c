#include "../lib/dragon.h"
#include "state.h"

bool stateOpBumpCollision(const State *state, CollPen p) {
    if (p.is && state->hero.vVel <= 0 && p.south > 0) {
        return true;
    }
    return false;
}

bool stateOpGameOver(const State *state) {
    bool allCoinsTaken = true;
    for (size_t i = 0; i < state->coin.n; ++i) {
        if (!state->coin.arr[i].taken) {
            allCoinsTaken = false;
        }
    }

    CollPen p = collBmpRect(state->lvl, state->hero.r);
    bool crushed = (p.south > 0 && p.north > 0) || (p.west > 0 && p.east > 0);

    return allCoinsTaken || crushed;
}
