typedef struct {
    float center[2], focus[2], zoom, angle, ar;
} Cam;

static inline Cam cam(float ar) {
    return (Cam){{0,0},{0,0},1,0,ar};
}

static inline float camGetW(const Cam *c) {
    return c->ar * 2 / c->zoom;
}

static inline void camSetW(Cam *c, float w) {
    c->zoom = fabsf(c->ar * 2 / w);
}

static inline float camGetH(const Cam *c) {
    return 2 / c->zoom;
}

static inline void camSetH(Cam *c, float h) {
    c->zoom = fabsf(2 / h);
}

static inline void camGetSize(const Cam *c, float size[2]) {
    size[0] = camGetW(c);
    size[1] = camGetH(c);
}

static inline void camSetMinSize(Cam *c, float w, float h) {
    w = fabsf(w);
    h = fabsf(h);
    c->zoom = c->ar * 2 / w < 2 / h ? c->ar * 2 / w : 2 / h;
}

static inline void camSetMaxSize(Cam *c, float w, float h) {
    w = fabsf(w);
    h = fabsf(h);
    c->zoom = c->ar * 2 / w > 2 / h ? c->ar * 2 / w : 2 / h;
}

static inline void camSetMinRect(Cam *c, float x, float y, float w, float h) {
    c->center[0] = c->focus[0] = x + w / 2;
    c->center[1] = c->focus[1] = y + h / 2;
    camSetMinSize(c, w, h);
}

static inline void camSetMaxRect(Cam *c, float x, float y, float w, float h) {
    c->center[0] = c->focus[0] = x + w / 2;
    c->center[1] = c->focus[1] = y + h / 2;
    camSetMaxSize(c, w, h);
}

static inline void camGetRect(const Cam *c, float rect[4]) {
    rect[2] = camGetW(c);
    rect[3] = camGetH(c);
    rect[0] = c->focus[0] - rect[2] / 2;
    rect[1] = c->focus[1] - rect[3] / 2;
}

static inline void camMatrix(const Cam *c, float m[4][4]) {
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
