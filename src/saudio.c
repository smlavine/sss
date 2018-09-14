#include "../lib/dragon.h"
#include "state.h"

static AudioSound *sound[STATE_OGG_COUNT - 1];
static AudioMusic *music;

void stateAudioInit(const char **oggPaths) {
    audioInit();
    for (int i = 0; i < STATE_OGG_COUNT - 1; ++i) {
        sound[i] = audioSoundLoad(oggPaths[i]);
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
    audioMusicStream(music);
    if (state->event[STATE_EVENT_WIN])
        audioSoundPlay(sound[STATE_OGG_WIN], false);
    else if (state->event[STATE_EVENT_DIE])
        audioSoundPlay(sound[STATE_OGG_DIE], false);
    else if (state->event[STATE_EVENT_EJECT])
        audioSoundPlay(sound[STATE_OGG_EJECT], false);
    else if (state->event[STATE_EVENT_JUMP])
        audioSoundPlay(sound[STATE_OGG_JUMP], false);
    else if (state->event[STATE_EVENT_COIN])
        audioSoundPlay(sound[STATE_OGG_COIN], false);
    else if (state->event[STATE_EVENT_GRAVITON])
        audioSoundPlay(sound[STATE_OGG_GRAVITON], false);
    else if (state->event[STATE_EVENT_KEY])
        audioSoundPlay(sound[STATE_OGG_KEY], false);
}
