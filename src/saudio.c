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

void stateAudioPlay(void) {
    audioMusicStream(music);
    if (s.event[STATE_EVENT_WIN]) {
        audioSoundPlay(sound[STATE_OGG_WIN]);
    } else if (s.event[STATE_EVENT_DIE]) {
        audioSoundPlay(sound[STATE_OGG_DIE]);
    } else if (s.event[STATE_EVENT_EJECT]) {
        audioSoundPlay(sound[STATE_OGG_EJECT]);
    } else if (s.event[STATE_EVENT_JUMP]) {
        audioSoundPlay(sound[STATE_OGG_JUMP]);
    } else if (s.event[STATE_EVENT_COIN]) {
        audioSoundPlay(sound[STATE_OGG_COIN]);
    } else if (s.event[STATE_EVENT_GRAVITON]) {
        audioSoundPlay(sound[STATE_OGG_GRAVITON]);
    } else if (s.event[STATE_EVENT_KEY]) {
        audioSoundPlay(sound[STATE_OGG_KEY]);
    }
}
