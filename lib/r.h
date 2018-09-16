typedef enum {
    R_CAPABILITY_DEPTH_TEST,
    R_CAPABILITY_TRANSPARENCY
} RCapability;

typedef enum {
    R_DRAW_MODE_POINTS,
    R_DRAW_MODE_LINES,
    R_DRAW_MODE_LINE_LOOP,
    R_DRAW_MODE_TRIANGLES,
    R_DRAW_MODE_TRIANGLE_STRIP,
    R_DRAW_MODE_TRIANGLE_FAN
} RDrawMode;

typedef struct {
    float x, y, z;
    unsigned char r, g, b, a;
} RVertex;

void rInit(void);
void rExit(void);
bool rCapability(RCapability capability, bool enabled);
void rPipeModel(const float m[4][4],const float c[4],float M[4][4],float C[4]);
void rPipeSpace(const float m[4][4],const float c[4],float M[4][4],float C[4]);
void rPipeWorld(const float m[4][4],const float c[4],float M[4][4],float C[4]);
void rClear(const float c[4]);
void rDraw(RDrawMode mode, size_t n, const RVertex *v);
void rDrawIndexed(RDrawMode mode,size_t ni,const uint16_t *i,const RVertex *v);
void rViewport(int x, int y, int w, int h);
