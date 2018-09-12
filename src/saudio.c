#include "../lib/dragon.h"
#include "state.h"

static AudioSound *sound[STATE_OGG_COUNT - 1];
static AudioMusic *music;

void stateAudioInit(const char **oggPaths) {
    audioInit();
    for (int i = 1; i < STATE_OGG_COUNT; ++i) {
        sound[i - 1] = audioSoundLoad(oggPaths[i]);
    }
    music = audioMusicLoad(oggPaths[STATE_OGG_MUSIC]);
    audioMusicPlay(music, true);
}

void stateAudioExit(void) {
    audioMusicStop(music);
    audioMusicFree(music);
    for (int i = 0; i < STATE_OGG_COUNT - 1; ++i) {
        audioSoundFree(sound[i]);
    }
    audioExit();
}

void stateAudioPlay(const State *state) {
    (void) state;
    audioMusicStream(music);
}
