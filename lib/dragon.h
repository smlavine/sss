#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// ---------------------------------------------------------------------------
// bmp.c
// ---------------------------------------------------------------------------

typedef struct {
    size_t w, h, l;
    uint8_t *b;
} Bmp;

Bmp *bmpNew(size_t w, size_t h, size_t l, Bmp *b);
Bmp *bmpDel(Bmp *bmp, bool freeHandle);
bool bmpGet(const Bmp *bmp, size_t x, size_t y, size_t z);
void bmpSet(Bmp *bmp, size_t x, size_t y, size_t z, bool b);

// ---------------------------------------------------------------------------
// coll.c
// ---------------------------------------------------------------------------

typedef struct {
    float x, y, w, h;
} CollRect;

typedef struct {
    bool is;
    float south, north, west, east;
} CollPen;

CollPen collBmpRect(const Bmp b, CollRect r);
CollPen collRect(CollRect a, CollRect b);

// ---------------------------------------------------------------------------
// r.c
// ---------------------------------------------------------------------------

typedef enum {
    R_DRAW_MODE_POINTS,
    R_DRAW_MODE_LINES,
    R_DRAW_MODE_LINE_LOOP,
    R_DRAW_MODE_TRIANGLES,
    R_DRAW_MODE_TRIANGLE_STRIP,
    R_DRAW_MODE_TRIANGLE_FAN
} RDrawMode;

typedef struct {
    float x, y;
    uint8_t r, g, b, a;
} RVertex;

void rInit(void);
void rExit(void);
void rPipe(float mulX, float mulY, float addX, float addY, float divX);
void rClear(const float *c);
void rDrawIndexed(RDrawMode mode,size_t ni,const uint32_t *i,const RVertex *v);
void rViewport(int x, int y, int w, int h);

// ---------------------------------------------------------------------------
// batch.c
// ---------------------------------------------------------------------------

typedef struct {
    float w, h, l;
    size_t ni, mi, nv, mv;
    uint32_t *i;
    RVertex *v;
} Batch;

Batch *batchDel(Batch *b, bool freeHandle);
void batch(Batch *b, size_t ni, const uint32_t *i, size_t nv, const RVertex *v);
void batchClear(Batch *b);
void batchRect(Batch *b, CollRect r, const uint8_t rgba[4]);
void batchLine(Batch *b, float x, float y, float angle, float len, float t, const uint8_t rgba[4]);
void batchRectLine(Batch *b, CollRect r, float ti, float to, const uint8_t rgba[4]);

// ---------------------------------------------------------------------------
// audio.c
// ---------------------------------------------------------------------------

typedef struct AudioSound AudioSound;
typedef struct AudioMusic AudioMusic;

void audioInit(void);
void audioExit(void);
AudioSound *audioSoundLoad(const char *path);
void audioSoundFree(AudioSound *sound);
void audioSoundPlay(const AudioSound *sound);
void audioSoundStop(const AudioSound *sound);
bool audioSoundPlaying(const AudioSound *sound);
AudioMusic *audioMusicLoad(const char *path);
void audioMusicFree(AudioMusic *music);
void audioMusicPlay(AudioMusic *music, bool repeat);
void audioMusicStream(AudioMusic *music);
void audioMusicPause(const AudioMusic *music);
void audioMusicStop(const AudioMusic *music);
bool audioMusicPlaying(const AudioMusic *music);
