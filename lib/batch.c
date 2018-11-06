#include "lib.h"

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

void batchRect(Batch *b, CollRect r, const uint8_t *rgb) {
    const uint32_t i[] = {
        (uint32_t)b->nv + 0,
        (uint32_t)b->nv + 1,
        (uint32_t)b->nv + 2,
        (uint32_t)b->nv + 2,
        (uint32_t)b->nv + 3,
        (uint32_t)b->nv + 0
    };
    const RVertex v[] = {
        {r.x,       r.y,       rgb[0], rgb[1], rgb[2]},
        {r.x + r.w, r.y,       rgb[0], rgb[1], rgb[2]},
        {r.x + r.w, r.y + r.h, rgb[0], rgb[1], rgb[2]},
        {r.x,       r.y + r.h, rgb[0], rgb[1], rgb[2]}
    };
    batch(b, 6, i, 4, v);
}

void batchRectLine(Batch *b,CollRect r,float ti,float to,const uint8_t *rgb) {
    batchRect(b, (CollRect){r.x - to,   r.y,        ti + to, r.h    }, rgb);
    batchRect(b, (CollRect){r.x,        r.y - to,   r.w,     ti + to}, rgb);
    batchRect(b, (CollRect){r.x,        r.y+r.h-ti, r.w,     ti + to}, rgb);
    batchRect(b, (CollRect){r.x+r.w-ti, r.y,        ti + to, r.h    }, rgb);
}
