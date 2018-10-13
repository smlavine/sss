#include "dragon.h"

#include <string.h>
#include <stdlib.h>

Bmp *bmpNew(size_t w, size_t h, size_t l, Bmp *b) {
    b = b ? b : malloc(sizeof(*b));
    b->w = w;
    b->h = h;
    b->l = l;
    size_t n = w * h * l % 8 ? w * h * l / 8 + 1 : w * h * l / 8;
    b->b = malloc(n);
    return b;
}

Bmp *bmpDel(Bmp *bmp, bool freeHandle) {
    free(bmp->b);
    if (freeHandle) {
        free(bmp);
    }
    return NULL;
}

bool bmpGet(const Bmp *bmp, size_t x, size_t y, size_t z) {
    size_t i = z * bmp->w * bmp->h + y * bmp->w + x;
    uint8_t byte = bmp->b[i / 8];
    uint8_t mask = 1 << (i % 8);
    return byte & mask;
}

void bmpSet(Bmp *bmp, size_t x, size_t y, size_t z, bool b) {
    size_t i = z * bmp->w * bmp->h + y * bmp->w + x;
    uint8_t byte = bmp->b[i / 8];
    uint8_t mask = 1 << (i % 8);
    bmp->b[i / 8] =  b ? byte | mask : byte & ~mask;
}
