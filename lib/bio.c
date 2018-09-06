#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "bio.h"

double bioF32(BioF32 n) {
    double sign = n.signandinteger >> 31 ? -1 : 1;
    double integer = (n.signandinteger << 1) >> 1;
    double fraction = n.fraction / (UINT32_MAX + 1.0);
    return sign * (integer + fraction);
}

BioF32 bioToF32(double n) {
    uint32_t sign = (n >= 0) ? 0 : 1;
    n = fabs(n);
    uint32_t integer = (uint32_t)n;
    uint32_t fraction = (uint32_t)((n - integer) * (UINT32_MAX + 1.0) + 0.5);
    uint32_t signandinteger = (sign << 31) + integer;
    return (BioF32){signandinteger, fraction};
}

const void *bioReadU8(const void *p, uint8_t *x) {
    *x = *(const uint8_t*)p;
    return (const uint8_t*)p + 1;
}

const void *bioReadU8v(const void *p, size_t n, uint8_t *x) {
    memcpy(x, p, n);
    return (const uint8_t*)p + n;
}

const void *bioReadU32LE(const void *p, uint32_t *x) {
    const uint8_t *n = p;
    *x = 0;
    for (int i = 0; i < 4; ++i) {
        *x += (uint32_t)(n[i] << 8 * i);
    }
    return n + 4;
}

const void *bioReadF32LE(const void *p, BioF32 *x) {
    p = bioReadU32LE(p, &x->signandinteger);
    p = bioReadU32LE(p, &x->fraction);
    return p;
}

void *bioWriteU8(void *p, uint8_t x) {
    *(uint8_t*)p = x;
    return (uint8_t*)p + 1;
}

void *bioWriteU8v(void *p, size_t n, const uint8_t *x) {
    memcpy(p, x, n);
    return (uint8_t*)p + n;
}

void *bioWriteU32LE(void *p, uint32_t x) {
    uint8_t *n = p;
    for (int i = 0; i < 2; ++i) {
        n[i] = (uint8_t)((x >> i * 8) % 256);
    }
    return n + 2;
}

void *bioWriteF32LE(void *p, BioF32 x) {
    p = bioWriteU32LE(p, x.signandinteger);
    p = bioWriteU32LE(p, x.fraction);
    return p;
}

uint8_t bioScanU8(void *f) {
    uint8_t x;
    fread(&x, 1, 1, f);
    return x;
}

void bioScanU8v(void *f, size_t n, uint8_t *x) {
    fread(x, n, 1, f);
}

uint32_t bioScanU32LE(void *f) {
    uint32_t x = 0;
    uint8_t n[4];
    fread(n, 4, 1, f);
    for (int i = 0; i < 4; ++i) {
        x += (uint32_t)(n[i] << i * 8);
    }
    return x;
}

BioF32 bioScanF32LE(void *f) {
    BioF32 n;
    n.signandinteger = bioScanU32LE(f);
    n.fraction = bioScanU32LE(f);
    return n;
}

void bioPrintU8(void *f, uint8_t x) {
    fwrite(&x, 1, 1, f);
}

void bioPrintU8v(void *f, size_t n, const uint8_t *x) {
    fwrite(x, n, 1, f);
}

void bioPrintU32LE(void *f, uint32_t x) {
    uint8_t n[4];
    for (int i = 0; i < 4; ++i) {
        n[i] = (uint8_t)((x >> i * 8) % 256);
    }
    fwrite(n, 4, 1, f);
}

void bioPrintF32LE(void *f, BioF32 x) {
    bioPrintU32LE(f, x.signandinteger);
    bioPrintU32LE(f, x.fraction);
}
