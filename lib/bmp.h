typedef struct {
    size_t w, h, l;
    uint8_t *b;
} Bmp;

static inline Bmp *bmpNew(size_t w, size_t h, size_t l, Bmp *b) {
    b = b ? b : malloc(sizeof(*b));
    b->w = w;
    b->h = h;
    b->l = l;
    size_t n = w * h * l % 8 ? w * h * l / 8 + 1 : w * h * l / 8;
    b->b = malloc(n);
    return b;
}

static inline Bmp *bmpDup(const Bmp *s, Bmp *d) {
    d = d ? d : malloc(sizeof(*d));
    memcpy(d, s, sizeof(*d));
    size_t w = d->w, h = d->h, l = d->l;
    size_t n = w * h * l % 8 ? w * h * l / 8 + 1 : w * h * l / 8;
    d->b = memcpy(malloc(n), s->b, n);
    return d;
}

static inline Bmp *bmpDel(Bmp *bmp, bool freeHandle) {
    free(bmp->b);
    if (freeHandle) {
        free(bmp);
    }
    return NULL;
}

static inline bool bmpGet(const Bmp *bmp, size_t x, size_t y, size_t z) {
    size_t i = z * bmp->w * bmp->h + y * bmp->w + x;
    uint8_t byte = i / 8;
    uint8_t bit = i % 8;
    uint8_t mask = 1 << bit;
    return bmp->b[byte] & mask;
}

static inline void bmpSet(Bmp *bmp, size_t x, size_t y, size_t z, bool b) {
    size_t i = z * bmp->w * bmp->h + y * bmp->w + x;
    uint8_t byte = i / 8;
    uint8_t bit = i % 8;
    uint8_t mask = 1 << bit;
    bmp->b[byte] =  b ? bmp->b[byte] | mask : bmp->b[byte] & ~mask;
}
