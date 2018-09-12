#include "../lib/dragon.h"
#include "state.h"

static AudioSound *sound[STATE_OGG_COUNT];

void stateAudioInit(const char **oggPaths) {
    audioInit();
    for (int i = 0; i < STATE_OGG_COUNT; ++i) {
        sound[i] = audioSoundLoad(oggPaths[i]);
    }
}

void stateAudioExit(void) {
    for (int i = 0; i < STATE_OGG_COUNT; ++i) {
        audioSoundFree(sound[i]);
    }
    audioExit();
}

void stateAudioPlay(const State *state) {
    (void) state;
}
