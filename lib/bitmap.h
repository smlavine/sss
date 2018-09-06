typedef struct {
    uint32_t w, h, l;
    uint8_t *b;
} Bitmap;

Bitmap *bitmapNew(uint32_t w, uint32_t h, uint32_t l, Bitmap *b);
Bitmap *bitmapCopy(const Bitmap *src, Bitmap *dst);
Bitmap *bitmapDel(Bitmap *bitmap, bool freeHandle);
Bitmap *bitmapRead(const void *p, Bitmap *bitmap);
void *bitmapWrite(const Bitmap *bitmap, void *p);
Bitmap *bitmapLoad(const char *path, Bitmap *bitmap);
void bitmapSave(const Bitmap *bitmap, const char *path);
Bitmap *bitmapLoadFromFile(void *file, bool close, Bitmap *bitmap);
void bitmapSaveToFile(const Bitmap *bitmap, void *file, bool close);
size_t bitmapSize(const Bitmap *bitmap);
bool bitmapGet(const Bitmap *bitmap, size_t x, size_t y, size_t z);
void bitmapSet(Bitmap *bitmap, size_t x, size_t y, size_t z, bool b);
