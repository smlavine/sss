typedef struct {
    uint8_t r, g, b, a;
} ImagePixel;

typedef struct {
    char *filename;
    uint32_t w, h;
    ImagePixel *p;
} Image;

Image *imageCopy(const Image *src, Image *dst);
Image *imageDel(Image *image, bool freeHandle);
Image *imageRead(const void *p, Image *image);
void *imageWrite(const Image *image, void *p);
Image *imageLoad(const char *path, Image *image);
void imageSave(const Image *image, const char *path);
Image *imageLoadFromFile(void *file, bool close, Image *image);
void imageSaveToFile(const Image *image, void *file, bool close);
size_t imageSize(const Image *image);
void imageSet(Image *image, uint32_t x, uint32_t y, ImagePixel p);
ImagePixel imageGet(const Image *image, uint32_t x, uint32_t y);
