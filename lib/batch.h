typedef struct {
    RendererTexture *t;
    size_t ni, mi, nv, mv;
    uint16_t *i;
    RendererVertex *v;
} BatchDrawCall;

typedef struct {
    float w, h, l;
    size_t n, m;
    BatchDrawCall *array;
} Batch;

Batch *batchCopy(const Batch *src, Batch *dst);
Batch *batchDel(Batch *batch, bool freeHandle);
void batch(Batch *batch, size_t n, const BatchDrawCall *c);
void batchClear(Batch *batch);
void batchDraw(const Batch *batch, RendererDrawMode mode);

BatchDrawCall *batchDrawCallCopy(const BatchDrawCall *src, BatchDrawCall *dst);
BatchDrawCall *batchDrawCallDel(BatchDrawCall *call, bool freeHandle);
void batchDrawCall(BatchDrawCall *call, size_t ni, const uint16_t *i, size_t nv, const RendererVertex *v);
void batchDrawCallClear(BatchDrawCall *call);
void batchDrawCallRect2D(BatchDrawCall *call, float x, float y, float w, float h, const uint8_t rgba[4]);
void batchDrawCallLine2D(BatchDrawCall *call, float x1, float y1, float x2, float y2, float t, const uint8_t rgba[4]);
