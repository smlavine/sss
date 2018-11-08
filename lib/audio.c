#include "lib.h"

#include <stdlib.h>

#include <AL/al.h>
#include <AL/alc.h>
#include <vorbis/vorbisfile.h>

#define AUDIO_CHANNEL_COUNT 2
#define AUDIO_FORMAT AL_FORMAT_STEREO16
#define AUDIO_RATE 44100
#define AUDIO_SAMPLE_SIZE 2
#define AUDIO_IS_BIG_ENDIAN 0
#define AUDIO_IS_SIGNED 1
#define MUSIC_BUFFER_COUNT 3
#define MUSIC_BUFFER_SIZE (AUDIO_RATE * AUDIO_CHANNEL_COUNT * AUDIO_SAMPLE_SIZE)

struct AudioSound {
    ALuint source, buffer;
};

struct AudioMusic {
    ALuint source, buffers[MUSIC_BUFFER_COUNT];
    OggVorbis_File f;
    bool repeat;
};

static ALuint loadOGG(const char *path);
static size_t loadBuffer(void *buffer, size_t bufferSize, OggVorbis_File *f, bool repeat);

static struct {
    ALCdevice *device;
    ALCcontext *context;
    void *buffer;
} g;

void audioInit(void) {
    g.device = alcOpenDevice(NULL);
    g.context = alcCreateContext(g.device, NULL);
    alcMakeContextCurrent(g.context);
    g.buffer = malloc(MUSIC_BUFFER_SIZE);
}

void audioExit(void) {
    free(g.buffer);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(g.context);
    alcCloseDevice(g.device);
}

AudioSound *audioSoundLoad(const char *path) {
    AudioSound *sound = malloc(sizeof(*sound));
    alGenSources(1, &sound->source);
    sound->buffer = loadOGG(path);
    return sound;
}

void audioSoundFree(AudioSound *sound) {
    audioSoundStop(sound);
    alDeleteSources(1, &sound->source);
    alDeleteBuffers(1, &sound->buffer);
    free(sound);
}

void audioSoundPlay(const AudioSound *sound) {
    alSourcef(sound->source, AL_PITCH, 1);
    alSourcef(sound->source, AL_GAIN, 1);
    alSource3f(sound->source, AL_POSITION, 0, 0, 0);
    alSource3f(sound->source, AL_VELOCITY, 0, 0, 0);
    alSourcei(sound->source, AL_LOOPING, AL_FALSE);
    alSourcei(sound->source, AL_BUFFER, sound->buffer);
    alSourcePlay(sound->source);
}

void audioSoundStop(const AudioSound *sound) {
    alSourceStop(sound->source);
}

AudioMusic *audioMusicLoad(const char *path) {
    AudioMusic *music = malloc(sizeof(*music));
    alGenSources(1, &music->source);
    alGenBuffers(MUSIC_BUFFER_COUNT, music->buffers);
    ov_open_callbacks(fopen(path, "rb"), &music->f, NULL, 0, OV_CALLBACKS_DEFAULT);
    return music;
}

void audioMusicFree(AudioMusic *music) {
    audioMusicStop(music);
    ov_clear(&music->f);
    alDeleteSources(1, &music->source);
    alDeleteBuffers(MUSIC_BUFFER_COUNT, music->buffers);
    free(music);
}

void audioMusicPlay(AudioMusic *music, bool repeat) {
    music->repeat = repeat;
    for (int i = 0; i < MUSIC_BUFFER_COUNT; ++i) {
        alBufferData(music->buffers[i], AUDIO_FORMAT, NULL, 0, AUDIO_RATE);
    }
    alSourcef(music->source, AL_PITCH, 1);
    alSourcef(music->source, AL_GAIN, 1);
    alSource3f(music->source, AL_POSITION, 0, 0, 0);
    alSource3f(music->source, AL_VELOCITY, 0, 0, 0);
    alSourcei(music->source, AL_LOOPING, 0);
    alSourceQueueBuffers(music->source, MUSIC_BUFFER_COUNT, music->buffers);
    alSourcePlay(music->source);
}

void audioMusicStream(AudioMusic *music) {
    ALint processedBufferCount;
    alGetSourcei(music->source, AL_BUFFERS_PROCESSED, &processedBufferCount);
    if (processedBufferCount <= 0) {
        return;
    }

    ALuint buffer;
    alSourceUnqueueBuffers(music->source, 1, &buffer);
    size_t bytesLoaded = loadBuffer(g.buffer, MUSIC_BUFFER_SIZE, &music->f, music->repeat);
    alBufferData(buffer, AUDIO_FORMAT, g.buffer, bytesLoaded, AUDIO_RATE);
    alSourceQueueBuffers(music->source, 1, &buffer);
    alSourcePlay(music->source);
    if (bytesLoaded == MUSIC_BUFFER_SIZE && processedBufferCount > 1) audioMusicStream(music);
}

void audioMusicStop(const AudioMusic *music) {
    alSourceStop(music->source);
    alSourcei(music->source, AL_BUFFER, 0);
}

static ALuint loadOGG(const char *path) {
    OggVorbis_File vf;
    ov_open_callbacks(fopen(path, "rb"), &vf, NULL, 0, OV_CALLBACKS_DEFAULT);

    long offset = 0;
    long remaining = ov_pcm_total(&vf, -1) * AUDIO_CHANNEL_COUNT * AUDIO_SAMPLE_SIZE;
    long totalBytes = remaining * AUDIO_CHANNEL_COUNT * AUDIO_SAMPLE_SIZE;
    void *b = malloc(totalBytes);
    int currentSection;
    while (remaining > 0) {
        long ret = ov_read(&vf, b + offset, totalBytes, AUDIO_IS_BIG_ENDIAN, AUDIO_SAMPLE_SIZE, AUDIO_IS_SIGNED, &currentSection);
        offset += ret;
        remaining -= ret;
    }

    ov_clear(&vf);

    ALuint buffer;
    alGenBuffers(1, &buffer);
    alBufferData(buffer, AUDIO_FORMAT, b, totalBytes, AUDIO_RATE);

    free(b);

    return buffer;
}

static size_t loadBuffer(void *buffer, size_t bufferSize, OggVorbis_File *f, bool repeat) {
    int currentSection;
    size_t offset = 0;
    while (offset < bufferSize) {
        long ret = ov_read(f, buffer + offset, bufferSize - offset, 0, 2, 1, &currentSection);
        if (ret == 0) {
            if (repeat) ov_pcm_seek_lap(f, 0);
            else break;
        }
        offset += ret;
    }
    return offset;
}
