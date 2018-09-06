#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "bio.h"
#include "bmp.h"

static bool getbit(uint8_t byte, int i);
static uint8_t setbit(uint8_t byte, int i, bool value);

Bmp *bmpNew(uint32_t w, uint32_t h, uint32_t l, Bmp *b) {
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

Bmp *bmpCopy(const Bmp *src, Bmp *dst) {
    if (!dst) dst = malloc(sizeof(*dst));
    memcpy(dst, src, sizeof(*dst));

    size_t b = dst->w * dst->h * dst->l / 8;
    if (dst->w * dst->h * dst->l % 8) {
        ++b;
    }

    dst->b = memcpy(malloc(b), src->b, b);

    return dst;
}

Bmp *bmpDel(Bmp *bmp, bool freeHandle) {
    free(bmp->b);
    if (freeHandle) {
        free(bmp);
    }
    return NULL;
}

Bmp *bmpRead(const void *p, Bmp *bmp) {
    if (!bmp) bmp = malloc(sizeof(*bmp));

    p = bioReadU32LE(p, &bmp->w);
    p = bioReadU32LE(p, &bmp->h);
    p = bioReadU32LE(p, &bmp->l);

    size_t b = bmp->w * bmp->h * bmp->l / 8;
    if (bmp->w * bmp->h * bmp->l % 8) {
        ++b;
    }
    bmp->b = malloc(b);
    p = bioReadU8v(p, b, bmp->b);

    return bmp;
}

void *bmpWrite(const Bmp *bmp, void *p) {
    p = bioWriteU32LE(p, bmp->w);
    p = bioWriteU32LE(p, bmp->h);
    p = bioWriteU32LE(p, bmp->l);

    size_t b = bmp->w * bmp->h * bmp->l / 8;
    if (bmp->w * bmp->h * bmp->l % 8) {
        ++b;
    }

    p = bioWriteU8v(p, b, bmp->b);

    return p;
}

Bmp *bmpLoad(const char *path, Bmp *bmp) {
    return bmpLoadFromFile(fopen(path, "rb"), true, bmp);
}

void bmpSave(const Bmp *bmp, const char *path) {
    bmpSaveToFile(bmp, fopen(path, "wb"), true);
}

Bmp *bmpLoadFromFile(void *file, bool close, Bmp *bmp) {
    if (!bmp) bmp = malloc(sizeof(*bmp));

    bmp->w = bioScanU32LE(file);
    bmp->h = bioScanU32LE(file);
    bmp->l = bioScanU32LE(file);

    size_t b = bmp->w * bmp->h * bmp->l / 8;
    if (bmp->w * bmp->h * bmp->l % 8) {
        ++b;
    }
    bmp->b = malloc(b);
    bioScanU8v(file, b, bmp->b);

    if (close) {
        fclose(file);
    }

    return bmp;
}

void bmpSaveToFile(const Bmp *bmp, void *file, bool close) {
    bioPrintU32LE(file, bmp->w);
    bioPrintU32LE(file, bmp->h);
    bioPrintU32LE(file, bmp->l);

    size_t b = bmp->w * bmp->h * bmp->l / 8;
    if (bmp->w * bmp->h * bmp->l % 8) {
        ++b;
    }

    bioPrintU8v(file, b, bmp->b);

    if (close) {
        fclose(file);
    }
}

size_t bmpSize(const Bmp *bmp) {
    size_t b = bmp->w * bmp->h * bmp->l / 8;
    if (bmp->w * bmp->h * bmp->l % 8) {
        ++b;
    }
    return 4 * 3 + b;
}

bool bmpGet(const Bmp *bmp, size_t x, size_t y, size_t z) {
    size_t i = z * bmp->w * bmp->h + y * bmp->w + x;
    return getbit(bmp->b[i / 8], i % 8);
}

void bmpSet(Bmp *bmp, size_t x, size_t y, size_t z, bool b) {
    size_t i = z * bmp->w * bmp->h + y * bmp->w + x;
    bmp->b[i / 8] = setbit(bmp->b[i / 8], i % 8, b);
}

static bool getbit(uint8_t byte, int i) {
    return byte & (1 << i);
}

static uint8_t setbit(uint8_t byte, int i, bool value) {
    return value ? ((uint8_t)value << i) | byte : byte & ~(1 << i);
}
