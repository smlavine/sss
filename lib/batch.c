#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "image.h"
#include "renderer.h"
#include "batch.h"

Batch *batchCopy(const Batch *src, Batch *dst) {
    if (!dst) dst = malloc(sizeof(*dst));
    memcpy(malloc(sizeof(*dst)), src, sizeof(*dst));
    dst->array = memcpy(malloc(dst->m * sizeof(*dst->array)), src->array, dst->m * sizeof(*dst->array));
    for (size_t i = 0; i < dst->m; ++i) {
        dst->array[i].i = memcpy(malloc(dst->array[i].mi * sizeof(*dst->array[i].i)), src->array[i].i, dst->array[i].ni * sizeof(*dst->array[i].i));
        dst->array[i].v = memcpy(malloc(dst->array[i].mv * sizeof(*dst->array[i].v)), src->array[i].v, dst->array[i].nv * sizeof(*dst->array[i].v));
    }
    return dst;
}

Batch *batchDel(Batch *batch, bool freeHandle) {
    for (size_t i = 0; i < batch->m; ++i) {
        free(batch->array[i].v);
        free(batch->array[i].i);
    }
    free(batch->array);
    if (freeHandle) free(batch);
    return NULL;
}

void batch(Batch *batch, size_t n, const BatchDrawCall *c) {
    if (batch->n + n > batch->m) {
        batch->array = realloc(batch->array, (batch->n + n) * sizeof(*batch->array));
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

BatchDrawCall *batchDrawCallCopy(const BatchDrawCall *src, BatchDrawCall *dst) {
    if (!dst) dst = malloc(sizeof(*dst));
    memcpy(dst, src, sizeof(*dst));
    dst->i = memcpy(malloc(dst->mi * sizeof(*dst->i)), src->i, dst->ni * sizeof(*dst->i));
    dst->v = memcpy(malloc(dst->mv * sizeof(*dst->v)), src->v, dst->nv * sizeof(*dst->v));
    return dst;
}

BatchDrawCall *batchDrawCallDel(BatchDrawCall *call, bool freeHandle) {
    free(call->v);
    free(call->i);
    if (freeHandle) free(call);
    return NULL;
}

void batchDraw(const Batch *batch, RendererDrawMode mode) {
    for (size_t i = 0; i < batch->n; ++i) {
        rendererTextureActivate(batch->array[i].t);
        rendererDrawIndexed(mode, batch->array[i].ni, batch->array[i].i, batch->array[i].v);
    }
}

void batchDrawCall(BatchDrawCall *call, size_t ni, const uint16_t *i, size_t nv, const RendererVertex *v) {
    if (call->mi < call->ni + ni) {
        if (call->mi == 0) call->mi = 1;
        while (call->mi < call->ni + ni) call->mi *= 2;
        call->i = realloc(call->i, call->mi * sizeof(*call->i));
    }
    if (i != NULL) {
        memcpy(call->i + call->ni, i, ni * sizeof(*i));
        call->ni += ni;
    }

    if (call->mv < call->nv + nv) {
        if (call->mv == 0) call->mv = 1;
        while (call->mv < call->nv + nv) call->mv *= 2;
        call->v = realloc(call->v, call->mv * sizeof(*call->v));
    }
    if (v != NULL) {
        memcpy(call->v + call->nv, v, nv * sizeof(*v));
        call->nv += nv;
    }
}

void batchDrawCallClear(BatchDrawCall *call) {
    call->ni = 0;
    call->nv = 0;
}

void batchDrawCallRect2D(BatchDrawCall *call, float x, float y, float w, float h, const uint8_t rgba[4]) {
    const uint16_t i[] = {
        (uint16_t)call->nv + 0,
        (uint16_t)call->nv + 1,
        (uint16_t)call->nv + 2,
        (uint16_t)call->nv + 2,
        (uint16_t)call->nv + 3,
        (uint16_t)call->nv + 0
    };
    const RendererVertex v[] = {
        {x,     y,     0, 0, 0, rgba[0], rgba[1], rgba[2], rgba[3]},
        {x + w, y,     0, 0, 0, rgba[0], rgba[1], rgba[2], rgba[3]},
        {x + w, y + h, 0, 0, 0, rgba[0], rgba[1], rgba[2], rgba[3]},
        {x,     y + h, 0, 0, 0, rgba[0], rgba[1], rgba[2], rgba[3]}
    };
    batchDrawCall(call, 6, i, 4, v);
}

void batchDrawCallLine2D(BatchDrawCall *call, float x1, float y1, float x2, float y2, float t, const uint8_t rgba[4]) {
    float angle = atan2f(y2 - y1, x2 - x1);
    float dx = sinf(angle) * t / 2;
    float dy = cosf(angle) * t / 2;
    const uint16_t i[] = {
        (uint16_t)call->nv + 0,
        (uint16_t)call->nv + 1,
        (uint16_t)call->nv + 2,
        (uint16_t)call->nv + 2,
        (uint16_t)call->nv + 3,
        (uint16_t)call->nv + 0
    };
    const RendererVertex v[] = {
        {x1 - dx, y1 + dy, 0, 0, 0, rgba[0], rgba[1], rgba[2], rgba[3]},
        {x1 + dx, y1 - dy, 0, 0, 0, rgba[0], rgba[1], rgba[2], rgba[3]},
        {x2 + dx, y2 - dy, 0, 0, 0, rgba[0], rgba[1], rgba[2], rgba[3]},
        {x2 - dx, y2 + dy, 0, 0, 0, rgba[0], rgba[1], rgba[2], rgba[3]}
    };
    batchDrawCall(call, 6, i, 4, v);
}
