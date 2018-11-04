#include "dragon.h"

#include <string.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.1415936525

Batch *batchDel(Batch *b, bool freeHandle) {
    free(b->v);
    free(b->i);
    if (freeHandle) {
        free(b);
    }
    return NULL;
}

void batch(Batch *b, size_t ni, const uint32_t *i, size_t nv, const RVertex *v) {
    if (b->mi < b->ni + ni) {
        if (b->mi == 0) {
            b->mi = 1;
        }
        while (b->mi < b->ni + ni) {
            b->mi *= 2;
        }
        b->i = realloc(b->i, b->mi * sizeof(*b->i));
    }
    if (i != NULL) {
        memcpy(b->i + b->ni, i, ni * sizeof(*i));
        b->ni += ni;
    }

    if (b->mv < b->nv + nv) {
        if (b->mv == 0) {
            b->mv = 1;
        }
        while (b->mv < b->nv + nv) {
            b->mv *= 2;
        }
        b->v = realloc(b->v, b->mv * sizeof(*b->v));
    }
    if (v != NULL) {
        memcpy(b->v + b->nv, v, nv * sizeof(*v));
        b->nv += nv;
    }
}

void batchClear(Batch *b) {
    b->ni = 0;
    b->nv = 0;
}

void batchRect(Batch *b, CollRect r, const uint8_t rgba[4]) {
    const uint32_t i[] = {
        (uint32_t)b->nv + 0,
        (uint32_t)b->nv + 1,
        (uint32_t)b->nv + 2,
        (uint32_t)b->nv + 2,
        (uint32_t)b->nv + 3,
        (uint32_t)b->nv + 0
    };
    const RVertex v[] = {
        {r.x,       r.y,       rgba[0], rgba[1], rgba[2], rgba[3]},
        {r.x + r.w, r.y,       rgba[0], rgba[1], rgba[2], rgba[3]},
        {r.x + r.w, r.y + r.h, rgba[0], rgba[1], rgba[2], rgba[3]},
        {r.x,       r.y + r.h, rgba[0], rgba[1], rgba[2], rgba[3]}
    };
    batch(b, 6, i, 4, v);
}

void batchLine(Batch *b, float x, float y, float angle, float len, float t, const uint8_t rgba[4]) {
    float dx = sinf(angle) * t / 2;
    float dy = cosf(angle) * t / 2;
    float x2 = x + cosf(angle) * len;
    float y2 = y + sinf(angle) * len;
    const uint32_t i[] = {
        (uint32_t)b->nv + 0,
        (uint32_t)b->nv + 1,
        (uint32_t)b->nv + 2,
        (uint32_t)b->nv + 2,
        (uint32_t)b->nv + 3,
        (uint32_t)b->nv + 0
    };
    const RVertex v[] = {
        {x - dx,   y + dy,  rgba[0], rgba[1], rgba[2], rgba[3]},
        {x + dx,   y - dy,  rgba[0], rgba[1], rgba[2], rgba[3]},
        {x2 + dx,  y2 - dy, rgba[0], rgba[1], rgba[2], rgba[3]},
        {x2 - dx,  y2 + dy, rgba[0], rgba[1], rgba[2], rgba[3]}
    };
    batch(b, 6, i, 4, v);
}

void batchRectLine(Batch *b, CollRect r, float ti, float to, const uint8_t rgba[4]) {
    float t = ti + to;
    float dt = (ti - to) / 2;
    batchLine(b, r.x,            r.y + dt,       0     , r.w, t, rgba);
    batchLine(b, r.x,            r.y + r.h - dt, 0     , r.w, t, rgba);
    batchLine(b, r.x + dt,       r.y,            PI / 2, r.h, t, rgba);
    batchLine(b, r.x + r.w - dt, r.y,            PI / 2, r.h, t, rgba);
}
