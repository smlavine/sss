typedef struct {
    uint32_t w, h, l;
    uint8_t *b;
} Bmp;

Bmp *bmpNew(uint32_t w, uint32_t h, uint32_t l, Bmp *b);
Bmp *bmpCopy(const Bmp *src, Bmp *dst);
Bmp *bmpDel(Bmp *bmp, bool freeHandle);
bool bmpGet(const Bmp *bmp, size_t x, size_t y, size_t z);
void bmpSet(Bmp *bmp, size_t x, size_t y, size_t z, bool b);
