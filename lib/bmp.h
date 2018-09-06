typedef struct {
    uint32_t w, h, l;
    uint8_t *b;
} Bmp;

Bmp *bmpNew(uint32_t w, uint32_t h, uint32_t l, Bmp *b);
Bmp *bmpCopy(const Bmp *src, Bmp *dst);
Bmp *bmpDel(Bmp *bmp, bool freeHandle);
Bmp *bmpRead(const void *p, Bmp *bmp);
void *bmpWrite(const Bmp *bmp, void *p);
Bmp *bmpLoad(const char *path, Bmp *bmp);
void bmpSave(const Bmp *bmp, const char *path);
Bmp *bmpLoadFromFile(void *file, bool close, Bmp *bmp);
void bmpSaveToFile(const Bmp *bmp, void *file, bool close);
size_t bmpSize(const Bmp *bmp);
bool bmpGet(const Bmp *bmp, size_t x, size_t y, size_t z);
void bmpSet(Bmp *bmp, size_t x, size_t y, size_t z, bool b);
