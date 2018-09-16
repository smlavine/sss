#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "r.h"
#include "bmp.h"
#include "coll.h"
#include "batch.h"

Batch *batchDup(const Batch *s, Batch *d) {
    if (!d) {
        d = malloc(sizeof(*d));
    }
    memcpy(d, s, sizeof(*d));
    d->i = memcpy(malloc(d->mi * sizeof(*d->i)), s->i, d->ni * sizeof(*d->i));
    d->v = memcpy(malloc(d->mv * sizeof(*d->v)), s->v, d->nv * sizeof(*d->v));
    return d;
}

Batch *batchDel(Batch *b, bool freeHandle) {
    free(b->v);
    free(b->i);
    if (freeHandle) {
        free(b);
    }
    return NULL;
}

void batch(Batch *b, size_t ni, const uint16_t *i, size_t nv, const RVertex *v) {
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
    const uint16_t i[] = {
        (uint16_t)b->nv + 0,
        (uint16_t)b->nv + 1,
        (uint16_t)b->nv + 2,
        (uint16_t)b->nv + 2,
        (uint16_t)b->nv + 3,
        (uint16_t)b->nv + 0
    };
    const RVertex v[] = {
        {r.x,       r.y,       0, rgba[0], rgba[1], rgba[2], rgba[3]},
        {r.x + r.w, r.y,       0, rgba[0], rgba[1], rgba[2], rgba[3]},
        {r.x + r.w, r.y + r.h, 0, rgba[0], rgba[1], rgba[2], rgba[3]},
        {r.x,       r.y + r.h, 0, rgba[0], rgba[1], rgba[2], rgba[3]}
    };
    batch(b, 6, i, 4, v);
}

void batchLine(Batch *b, CollLine l, float t, const uint8_t rgba[4]) {
    float dx = sinf(l.angle) * t / 2;
    float dy = cosf(l.angle) * t / 2;
    float x2 = l.x + dx * l.len;
    float y2 = l.x + dy * l.len;
    const uint16_t i[] = {
        (uint16_t)b->nv + 0,
        (uint16_t)b->nv + 1,
        (uint16_t)b->nv + 2,
        (uint16_t)b->nv + 2,
        (uint16_t)b->nv + 3,
        (uint16_t)b->nv + 0
    };
    const RVertex v[] = {
        {l.x - dx, l.y + dy, 0, rgba[0], rgba[1], rgba[2], rgba[3]},
        {l.x + dx, l.y - dy, 0, rgba[0], rgba[1], rgba[2], rgba[3]},
        {x2 + dx, y2 - dy, 0, rgba[0], rgba[1], rgba[2], rgba[3]},
        {x2 - dx, y2 + dy, 0, rgba[0], rgba[1], rgba[2], rgba[3]}
    };
    batch(b, 6, i, 4, v);
}
