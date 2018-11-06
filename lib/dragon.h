#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// ===========================================================================

/*
 * A simple bitmap type.
 * Previously I saved memory by compressing eight bits into a byte.
 * I found it unnecessary because of how hard it is to make good huge levels.
 * w - the width of the bitmap. Valid range: [1; floor(sqrt(SIZE_MAX))].
 * h - the height of the bitmap. Valid range: [1; floor(sqrt(SIZE_MAX))].
 * b - a byte array where b[y*w+x] is a bit at coordinate (x;y).
 */

typedef struct {
    size_t w, h;
    uint8_t *b;
} Bmp;

static inline bool bmpGet(const Bmp b, size_t x, size_t y) {
    return b.b[y * b.w + x];
}

static inline void bmpSet(Bmp b, size_t x, size_t y, bool v) {
    b.b[y * b.w + x] = v;
}

// ===========================================================================

/*
 * Functions that deal with collisions.
 * Defined in lib/coll.c.
 */

/*
 * A simple rectangle type.
 * x - the x coordinate of the rectangle. Must be a normalized number or zero.
 * y - the y coordinate of the rectangle. Must be a normalized number or zero.
 * w - the width of the rectangle. Must be a normalized number or zero.
 *     If not positive then assumed non-existant.
 * h - the height of the rectangle. Must be a normalized number or zero.
 *     If not positive then assumed non-existant.
 */
typedef struct {
    float x, y, w, h;
} CollRect;

/*
 * A 2D penetration type.
 * Good enough to describe a 2D collision between two simple 2D shapes.
 * One shape is supposed to be immovable (static).
 * The other shape is supposed to be moved, altered or destroyed.
 *
 * is - whether the collision is currently occuring between the two shapes.
 * south - how much to move the object UP to resolve the collision.
 *         Must be a normalized positive number or zero (positive).
 * north - how much to move the object DOWN to resolve the collision.
 *         Must be a normalized positive number or zero (positive).
 * west - how much to move the object RIGHT to resolve the collision.
 *        Must be a normalized positive number or zero (positive).
 * east - how must to move the object LEFT to resolve the collision.
 *        Must be a normalized positive number or zero (positive).
 *
 * If either both south and north are more than zero or both west and east
 * are more than zero, then an irreconcilable collision has happened.
 * In that case no more information can be extracted from the structure:
 * the members of the structure should be assumed to have random values.
 */
typedef struct {
    bool is;
    float south, north, west, east;
} CollPen;

/*
 * Tries to resolve a collision between a bitmap and a rectangle.
 * The function won't report any collision if any corner of the rectangle is
 * outside of the bitmap or if the north or east edges are on top one another.
 * b - the bitmap. It's supposed to be immovable (static).
 * r - the rectangle. It's supposed to be dynamic.
 * Returns CollPen, describing the collision.
 */

CollPen collBmpRect(const Bmp b, CollRect r);

 /*
  * Resolves a collision between to rectangles.
  * a - the dynamic rectangle.
  * b - the static, immovable rectangle.
  * Returns CollPen, describing the collision, always reconcilable.
  */
CollPen collRect(CollRect a, CollRect b);

// ===========================================================================

typedef struct {
    float x, y;
    uint8_t r, g, b, a;
} RVertex;

void rInit(void);
void rExit(void);
void rPipe(float mulX, float mulY, float addX, float addY, float divX);
void rClear(const float *c);
void rDrawIndexedTriangles(size_t ni,const uint32_t *i,const RVertex *v);
void rViewport(int x, int y, int w, int h);

// ===========================================================================

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

// ===========================================================================

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
