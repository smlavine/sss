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

/*
 * The renderer is basically a cute tiny wrapper around OpenGL.
 * The screen is represented by a canvas stretching from -1 to 1 in both
 * directions. So let's say we want to draw a red square covering all screen.
 * Then we need to pass 4 vertices (points) and 6 indices to draw 2 triangles:
 * Points: (-1; -1), ( 1; -1), ( 1; 1), (-1; 1)
 * Indices: 0, 1, 2,  2, 3, 0
 * Or, in code...
 *
 *    RVertex v1 = {-1, -1, 255, 0, 0};
 *    RVertex v2 = { 1, -1, 255, 0, 0};
 *    RVertex v3 = { 1,  1, 255, 0, 0};
 *    RVertex v4 = {-1,  1, 255, 0, 0};
 *    RVertex v[] = {v1, v2, v3, v4};
 *    uint32_t i[] = {0, 1, 2,  2, 3, 0};
 *    rDrawIndexedTriangles(6, i, v);
 *
 * Now say we want to draw a red square from the middle of the screen to the
 * top right corner without changing any vertices. Let's use the "pipeline"!
 * You see, every vertex (point) goes through a pipeline. In that pipeline,
 * every X coordinate is multiplied my mulX, and every Y coordinate - by mulY.
 * After they have been multiplied, addX is added to X and addY - to Y.
 * By default, mulX and mulY are 1, addX and addY are 0. We can override that.
 * Let's set mulX = mulY = addX = addY = 0.5 before rDrawIndexedTriangles().
 *
 *     rPipe(0.5, 0.5, 0.5, 0.5);
 *     ...
 *
 * Tah-dah! There we have it.
 * Oh, and what I forgot to mention is that the screen is not the actual
 * screen, it's the virtual screen which is a portion of the real screen.
 * Let's say you have a window that is 800x600. Then the virtual screen
 * can be any rectangle on that window. You can (and should) set the virtual
 * screen to be exactly the same as the actual screen (x=0,y=0,w=800,h=600)
 * with rViewport() before drawing anything.
 *
 *     rViewport(0, 0, windowW, windowH); // virtual screen = actual screen
 *     ... // draw stuff
 */

/*
 * The vertex type. Basically a fancy name for a point.
 * x, y - the coordinates of the vertex.
 * r, g, b - the color of the vertex.
 */
typedef struct {
    float x, y;
    uint8_t r, g, b;
} RVertex;

/*
 * Initialize the renderer.
 * Prerequisite for all the other renderer functions.
 */
void rInit(void);

/*
 * Terminate the renderer.
 * Call after you're done with the renderer.
 */
void rExit(void);

/*
 * Change the values of the pipeline.
 * As explained, every vertex, before getting on the screen (which is a canvas
 * stretching from -1 to 1 in both directions) goes through a pipeline. In the
 * pipeline, X = X * mulX + addX and Y = Y * mulY + addY. This function thus
 * changes the values of mulX, mulY, addX and addY, which by default are 1, 1,
 * 0 and 0, accordingly.
 */
void rPipe(float mulX, float mulY, float addX, float addY);

/*
 * Clears the real, whole, actual screen with specified color.
 * r, g, b - the specified color to clear the screen with.
 */
void rClear(uint8_t r, uint8_t g, uint8_t b);

/*
 * Draws indexed triangles.
 * ni - number of indices.
 * i - array of indices that reference the vertices.
 * v - array of vertices.
 */
void rDrawIndexedTriangles(size_t ni, const uint32_t *i, const RVertex *v);

/*
 * Set the virtual screen rectangle, which is a portion of the actual screen.
 * Use rViewport(0, 0, windowW, windowH) to make it equal to the real screen.
 * x, y - the coordinates of the lower left corner of the virtual screen
          rectangle. By default both are 0.
 * w, h - the size of the virtual screen rectangle. By default are the initial
 *        size of the actual screen, but don't count on that.
 */
void rViewport(int x, int y, int w, int h);

// ===========================================================================

/*
 * A batch is two dynamic arrays - one of vertices and one of indices.
 *     typedef struct {
 *         vector<uint32_t> i;
 *         vector<RVertex> v;
 *     } Batch;
 * To allocate a new one simply calloc the structure or zero-out one on stack.
 * ni - the actual number of indices in the index array.
 * nv - the actual number of vertices in the vertex array.
 * mi - the number of indices the index array has memory to contain.
 * mv - the number of vertices the vertex array has memory to contain.
 * i - the index array.
 * v - the vertex array.
 */
typedef struct {
    size_t ni, mi, nv, mv;
    uint32_t *i;
    RVertex *v;
} Batch;

/*
 * Deletes a batch.
 * This is the only batch function that actually deallocates memory.
 * b - pointer to the batch to be deleted.
 * freeHandle - whether to free(b) as well.
 * Always returns NULL.
 */
Batch *batchDel(Batch *b, bool freeHandle);

/*
 * Batches indices and vertices.
 * b - the batch to batch (add) indices and vertices to.
 * ni - the number of indices to add to the batch.
 * i - the index array to batch the indices from or NULL to just allocate
       memory for at least ni additional indices (batch.ni isn't incremented).
 * nv - the number of vertices to add to the batch.
 * v - the vertex array to batch the vertices from or NULL to just allocate
 *     memory for at least nv additional vertices (batch.nv isn't incremented)
 */
void batch(Batch*b, size_t ni, const uint32_t*i, size_t nv, const RVertex*v);

/*
 * Assign batch.ni and batcn.nv to 0 (no memory is de-allocated).
 */
void batchClear(Batch *b);

/*
 * Batches four vertices and six indices to draw a rectangle.
 * b - the batch to batch the vertices and indices to.
 * r - the rectangle to draw.
 * rgb - the color of vertices.
 */
void batchRect(Batch *b, CollRect r, const uint8_t *rgb);

/*
 * Batches four rectangles to draw the outline of the rectangle.
 * b - the batch to batch the outline of of the rectangle to.
 * r - the rectangle to batch the outline of.
 * ti - the thickness of the lines going inside the specified rectangle.
 * to - the thickness of the lines going outside the specified rectangle.
 * rgb - the color of the vertices.
 */
void batchRectLine(Batch *b,CollRect r,float ti,float to,const uint8_t *rgb);

// ===========================================================================

typedef struct AudioSound AudioSound;
typedef struct AudioMusic AudioMusic;

void audioInit(void);
void audioExit(void);
AudioSound *audioSoundLoad(const char *path);
void audioSoundFree(AudioSound *sound);
void audioSoundPlay(const AudioSound *sound);
void audioSoundStop(const AudioSound *sound);
AudioMusic *audioMusicLoad(const char *path);
void audioMusicFree(AudioMusic *music);
void audioMusicPlay(AudioMusic *music, bool repeat);
void audioMusicStream(AudioMusic *music);
void audioMusicPause(const AudioMusic *music);
void audioMusicStop(const AudioMusic *music);
