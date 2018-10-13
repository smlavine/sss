#include "dragon.h"

#include <math.h>

Cam cam(float ar) {
    return (Cam){{0,0},{0,0},1,0,ar};
}

void camSetMinRect(Cam *c, CollRect r) {
    r.w = fabsf(r.w);
    r.h = fabsf(r.h);
    c->center[0] = c->focus[0] = r.x + r.w / 2;
    c->center[1] = c->focus[1] = r.y + r.h / 2;
    c->zoom = c->ar * 2 / r.w < 2 / r.h ? c->ar * 2 / r.w : 2 / r.h;
}

void camSetMaxRect(Cam *c, CollRect r) {
    r.w = fabsf(r.w);
    r.h = fabsf(r.h);
    c->center[0] = c->focus[0] = r.x + r.w / 2;
    c->center[1] = c->focus[1] = r.y + r.h / 2;
    c->zoom = c->ar * 2 / r.w > 2 / r.h ? c->ar * 2 / r.w : 2 / r.h;
}

CollRect camGetRect(const Cam *c) {
    CollRect r;
    r.w = c->ar * 2 / c->zoom;
    r.h = 2 / c->zoom;
    r.x = c->focus[0] - r.w / 2;
    r.y = c->focus[1] - r.h / 2;
    return r;
}

void camMatrix(const Cam *c, float m[4][4]) {
    float a[4][4], b[4][4], v[4];
    float offset[4] = {c->center[0]-c->focus[0],c->center[1]-c->focus[1],0,0};

    matrixTranslation(m, -c->center[0], -c->center[1], 0);
    matrixScale(a, c->zoom, c->zoom, 1);
    matrixProduct(b, a, m);

    matrixRotationZ(a, c->angle);
    matrixProduct(m, a, b);

    matrixVectorProduct(v, m, offset);
    matrixTranslation(a, v[0], v[1], v[2]);
    matrixProduct(b, a, m);

    matrixScale(a, 1 / c->ar, 1, 1);
    matrixProduct(m, a, b);
}
