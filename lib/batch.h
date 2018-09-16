typedef struct {
    float w, h, l;
    size_t ni, mi, nv, mv;
    uint16_t *i;
    RVertex *v;
} Batch;

// TODO: batchNew
Batch *batchCopy(const Batch *s, Batch *d);
Batch *batchDel(Batch *b, bool freeHandle);
void batch(Batch *b, size_t ni, const uint16_t *i, size_t nv, const RVertex *v);
void batchClear(Batch *b);
void batchRect(Batch *b, CollRect r, const uint8_t rgba[4]);
void batchLine(Batch *b, CollLine l, float t, const uint8_t rgba[4]);
