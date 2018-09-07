#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "image.h"
#include "r.h"
#include "bmp.h"
#include "coll.h"
#include "batch.h"

Batch *batchCopy(const Batch *src, Batch *dst) {
    if (!dst) {
        dst = malloc(sizeof(*dst));
    }
    memcpy(malloc(sizeof(*dst)), src, sizeof(*dst));
    size_t s = dst->m * sizeof(*dst->array);
    dst->array = memcpy(malloc(s), src->array, s);
    for (size_t i = 0; i < dst->m; ++i) {
        size_t si = dst->array[i].mi * sizeof(*dst->array[i].i);
        size_t sv = dst->array[i].mv * sizeof(*dst->array[i].v);
        dst->array[i].i = memcpy(malloc(si), src->array[i].i, si);
        dst->array[i].v = memcpy(malloc(sv), src->array[i].v, sv);
    }
    return dst;
}

Batch *batchDel(Batch *batch, bool freeHandle) {
    for (size_t i = 0; i < batch->m; ++i) {
        free(batch->array[i].v);
        free(batch->array[i].i);
    }
    free(batch->array);
    if (freeHandle) {
        free(batch);
    }
    return NULL;
}

void batch(Batch *batch, size_t n, const BatchCall *c) {
    if (batch->n + n > batch->m) {
        batch->array=realloc(batch->array,(batch->n+n)*sizeof(*batch->array));
        batch->m = batch->n + n;
    }
    if (c) {
        memcpy(batch->array + batch->n, c, n * sizeof(*batch->array));
        batch->n += n;
    }
}

void batchClear(Batch *batch) {
    batch->n = 0;
}

BatchCall *batchCallCopy(const BatchCall *s, BatchCall *d) {
    if (!d) {
        d = malloc(sizeof(*d));
    }
    memcpy(d, s, sizeof(*d));
    d->i = memcpy(malloc(d->mi * sizeof(*d->i)), s->i, d->ni * sizeof(*d->i));
    d->v = memcpy(malloc(d->mv * sizeof(*d->v)), s->v, d->nv * sizeof(*d->v));
    return d;
}

BatchCall *batchCallDel(BatchCall *call, bool freeHandle) {
    free(call->v);
    free(call->i);
    if (freeHandle) {
        free(call);
    }
    return NULL;
}

void batchDraw(const Batch *b, RDrawMode mode) {
    for (size_t i = 0; i < b->n; ++i) {
        rTexActivate(b->array[i].t);
        rDrawIndexed(mode, b->array[i].ni, b->array[i].i, b->array[i].v);
    }
}

void batchCall
(BatchCall*call,size_t ni,const uint16_t*i,size_t nv,const RVertex*v) {
    if (call->mi < call->ni + ni) {
        if (call->mi == 0) {
            call->mi = 1;
        }
        while (call->mi < call->ni + ni) {
            call->mi *= 2;
        }
        call->i = realloc(call->i, call->mi * sizeof(*call->i));
    }
    if (i != NULL) {
        memcpy(call->i + call->ni, i, ni * sizeof(*i));
        call->ni += ni;
    }

    if (call->mv < call->nv + nv) {
        if (call->mv == 0) {
            call->mv = 1;
        }
        while (call->mv < call->nv + nv) {
            call->mv *= 2;
        }
        call->v = realloc(call->v, call->mv * sizeof(*call->v));
    }
    if (v != NULL) {
        memcpy(call->v + call->nv, v, nv * sizeof(*v));
        call->nv += nv;
    }
}

void batchCallClear(BatchCall *call) {
    call->ni = 0;
    call->nv = 0;
}

void batchCallRect(BatchCall *call, CollRect r, const uint8_t rgba[4]) {
    const uint16_t i[] = {
        (uint16_t)call->nv + 0,
        (uint16_t)call->nv + 1,
        (uint16_t)call->nv + 2,
        (uint16_t)call->nv + 2,
        (uint16_t)call->nv + 3,
        (uint16_t)call->nv + 0
    };
    const RVertex v[] = {
        {r.x,       r.y,       0, 0, 0, rgba[0], rgba[1], rgba[2], rgba[3]},
        {r.x + r.w, r.y,       0, 0, 0, rgba[0], rgba[1], rgba[2], rgba[3]},
        {r.x + r.w, r.y + r.h, 0, 0, 0, rgba[0], rgba[1], rgba[2], rgba[3]},
        {r.x,       r.y + r.h, 0, 0, 0, rgba[0], rgba[1], rgba[2], rgba[3]}
    };
    batchCall(call, 6, i, 4, v);
}

void batchCallLine(BatchCall *call,CollLine l,float t,const uint8_t rgba[4]) {
    float dx = sinf(l.angle) * t / 2;
    float dy = cosf(l.angle) * t / 2;
    float x2 = l.x + dx * l.len;
    float y2 = l.x + dy * l.len;
    const uint16_t i[] = {
        (uint16_t)call->nv + 0,
        (uint16_t)call->nv + 1,
        (uint16_t)call->nv + 2,
        (uint16_t)call->nv + 2,
        (uint16_t)call->nv + 3,
        (uint16_t)call->nv + 0
    };
    const RVertex v[] = {
        {l.x - dx, l.y + dy, 0, 0, 0, rgba[0], rgba[1], rgba[2], rgba[3]},
        {l.x + dx, l.y - dy, 0, 0, 0, rgba[0], rgba[1], rgba[2], rgba[3]},
        {x2 + dx, y2 - dy, 0, 0, 0, rgba[0], rgba[1], rgba[2], rgba[3]},
        {x2 - dx, y2 + dy, 0, 0, 0, rgba[0], rgba[1], rgba[2], rgba[3]}
    };
    batchCall(call, 6, i, 4, v);
}
