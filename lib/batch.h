typedef struct {
    RTex *t;
    size_t ni, mi, nv, mv;
    uint16_t *i;
    RVertex *v;
} BatchCall;

typedef struct {
    float w, h, l;
    size_t n, m;
    BatchCall *array;
} Batch;

// TODO: batchNew
Batch *batchCopy(const Batch *src, Batch *dst);
Batch *batchDel(Batch *batch, bool freeHandle);
void batch(Batch *batch, size_t n, const BatchCall *c);
void batchClear(Batch *batch);
void batchDraw(const Batch *batch, RDrawMode mode);

// TODO: batchCallNew
BatchCall *batchCallCopy(const BatchCall *src, BatchCall *dst);
BatchCall *batchCallDel(BatchCall *call, bool freeHandle);
void batchCall
(BatchCall*call,size_t ni,const uint16_t*i,size_t nv,const RVertex*v);
void batchCallClear(BatchCall *c);
void batchCallRect(BatchCall *c, CollRect r, const uint8_t rgba[4]);
void batchCallLine(BatchCall *c, CollLine l, float t, const uint8_t rgba[4]);
