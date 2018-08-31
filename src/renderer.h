typedef enum {
    RENDERER_CAPABILITY_DEPTH_TEST,
    RENDERER_CAPABILITY_TRANSPARENCY
} RendererCapability;

typedef enum {
    RENDERER_DRAW_MODE_POINTS,
    RENDERER_DRAW_MODE_LINES,
    RENDERER_DRAW_MODE_LINE_LOOP,
    RENDERER_DRAW_MODE_TRIANGLES,
    RENDERER_DRAW_MODE_TRIANGLE_STRIP,
    RENDERER_DRAW_MODE_TRIANGLE_FAN
} RendererDrawMode;

typedef struct {
    float x, y, z, s, t;
    unsigned char r, g, b, a;
} RendererVertex;

typedef struct RendererTexture RendererTexture;

typedef enum {
    RENDERER_TEXTURE_QUALITY_WORST,
    RENDERER_TEXTURE_QUALITY_BEST
} RendererTextureQuality;

typedef enum {
    RENDERER_TEXTURE_WRAPPING_CLAMP_TO_EDGE,
    RENDERER_TEXTURE_WRAPPING_REPEAT,
    RENDERER_TEXTURE_WRAPPING_MIRRORED_REPEAT
} RendererTextureWrapping;

void rendererInit(void);
void rendererExit(void);
bool rendererCapability(RendererCapability capability, bool enabled);
void rendererPipelineModel(const float matrix[4][4], const float color[4], float prevMatrix[4][4], float prevColor[4]);
void rendererPipelineSpace(const float matrix[4][4], const float color[4], float prevMatrix[4][4], float prevColor[4]);
void rendererPipelineWorld(const float matrix[4][4], const float color[4], float prevMatrix[4][4], float prevColor[4]);
void rendererClear(const float color[4]);
void rendererDraw(RendererDrawMode mode, size_t n, const RendererVertex *v);
void rendererDrawIndexed(RendererDrawMode mode, size_t ni, const uint16_t *i, const RendererVertex *v);
void rendererViewport(int x, int y, int w, int h);
RendererTexture *rendererTextureNew(const Image *image, RendererTextureQuality quality, RendererTextureWrapping wrapping);
RendererTexture *rendererTextureDel(RendererTexture *texture);
void rendererTextureActivate(const RendererTexture *texture);
const RendererTexture *rendererTextureActive(void);
