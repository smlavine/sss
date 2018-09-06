#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "bio.h"
#include "bitmap.h"

static bool getbit(uint8_t byte, int i);
static uint8_t setbit(uint8_t byte, int i, bool value);

Bitmap *bitmapNew(uint32_t w, uint32_t h, uint32_t l, Bitmap *b) {
    if (!b) b = malloc(sizeof(b));
    b->w = w;
    b->h = h;
    b->l = l;
    size_t bytes = b->w * b->h * b->l / 8;
    if (b->w * b->h * b->l % 8) {
        ++bytes;
    }
    b->b = malloc(bytes);
    return b;
}

Bitmap *bitmapCopy(const Bitmap *src, Bitmap *dst) {
    if (!dst) dst = malloc(sizeof(*dst));
    memcpy(dst, src, sizeof(*dst));

    size_t b = dst->w * dst->h * dst->l / 8;
    if (dst->w * dst->h * dst->l % 8) {
        ++b;
    }

    dst->b = memcpy(malloc(b), src->b, b);

    return dst;
}

Bitmap *bitmapDel(Bitmap *bitmap, bool freeHandle) {
    free(bitmap->b);
    if (freeHandle) {
        free(bitmap);
    }
    return NULL;
}

Bitmap *bitmapRead(const void *p, Bitmap *bitmap) {
    if (!bitmap) bitmap = malloc(sizeof(*bitmap));

    p = bioReadU32LE(p, &bitmap->w);
    p = bioReadU32LE(p, &bitmap->h);
    p = bioReadU32LE(p, &bitmap->l);

    size_t b = bitmap->w * bitmap->h * bitmap->l / 8;
    if (bitmap->w * bitmap->h * bitmap->l % 8) {
        ++b;
    }
    bitmap->b = malloc(b);
    p = bioReadU8v(p, b, bitmap->b);

    return bitmap;
}

void *bitmapWrite(const Bitmap *bitmap, void *p) {
    p = bioWriteU32LE(p, bitmap->w);
    p = bioWriteU32LE(p, bitmap->h);
    p = bioWriteU32LE(p, bitmap->l);

    size_t b = bitmap->w * bitmap->h * bitmap->l / 8;
    if (bitmap->w * bitmap->h * bitmap->l % 8) {
        ++b;
    }

    p = bioWriteU8v(p, b, bitmap->b);

    return p;
}

Bitmap *bitmapLoad(const char *path, Bitmap *bitmap) {
    return bitmapLoadFromFile(fopen(path, "rb"), true, bitmap);
}

void bitmapSave(const Bitmap *bitmap, const char *path) {
    bitmapSaveToFile(bitmap, fopen(path, "wb"), true);
}

Bitmap *bitmapLoadFromFile(void *file, bool close, Bitmap *bitmap) {
    if (!bitmap) bitmap = malloc(sizeof(*bitmap));

    bitmap->w = bioScanU32LE(file);
    bitmap->h = bioScanU32LE(file);
    bitmap->l = bioScanU32LE(file);

    size_t b = bitmap->w * bitmap->h * bitmap->l / 8;
    if (bitmap->w * bitmap->h * bitmap->l % 8) {
        ++b;
    }
    bitmap->b = malloc(b);
    bioScanU8v(file, b, bitmap->b);

    if (close) {
        fclose(file);
    }

    return bitmap;
}

void bitmapSaveToFile(const Bitmap *bitmap, void *file, bool close) {
    bioPrintU32LE(file, bitmap->w);
    bioPrintU32LE(file, bitmap->h);
    bioPrintU32LE(file, bitmap->l);

    size_t b = bitmap->w * bitmap->h * bitmap->l / 8;
    if (bitmap->w * bitmap->h * bitmap->l % 8) {
        ++b;
    }

    bioPrintU8v(file, b, bitmap->b);

    if (close) {
        fclose(file);
    }
}

size_t bitmapSize(const Bitmap *bitmap) {
    size_t b = bitmap->w * bitmap->h * bitmap->l / 8;
    if (bitmap->w * bitmap->h * bitmap->l % 8) {
        ++b;
    }
    return 4 * 3 + b;
}

bool bitmapGet(const Bitmap *bitmap, size_t x, size_t y, size_t z) {
    size_t i = z * bitmap->w * bitmap->h + y * bitmap->w + x;
    return getbit(bitmap->b[i / 8], i % 8);
}

void bitmapSet(Bitmap *bitmap, size_t x, size_t y, size_t z, bool b) {
    size_t i = z * bitmap->w * bitmap->h + y * bitmap->w + x;
    bitmap->b[i / 8] = setbit(bitmap->b[i / 8], i % 8, b);
}

static bool getbit(uint8_t byte, int i) {
    return byte & (1 << i);
}

static uint8_t setbit(uint8_t byte, int i, bool value) {
    return value ? ((uint8_t)value << i) | byte : byte & ~(1 << i);
}
