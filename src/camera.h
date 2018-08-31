typedef struct {
    float center[2], focus[2], zoom, angle, ar;
} Camera2D;

static inline Camera2D camera2D(float ar) {
    return (Camera2D){{0,0},{0,0},1,0,ar};
}

static inline float camera2DgetWidth(const Camera2D *c) {
    return c->ar * 2 / c->zoom;
}

static inline void camera2DsetWidth(Camera2D *c, float w) {
    c->zoom = fabsf(c->ar * 2 / w);
}

static inline float camera2DgetHeight(const Camera2D *c) {
    return 2 / c->zoom;
}

static inline void camera2DsetHeight(Camera2D *c, float h) {
    c->zoom = fabsf(2 / h);
}

static inline void camera2DgetSize(const Camera2D *c, float size[2]) {
    size[0] = camera2DgetWidth(c);
    size[1] = camera2DgetHeight(c);
}

static inline void camera2DsetMinSize(Camera2D *c, float w, float h) {
    w = fabsf(w);
    h = fabsf(h);
    c->zoom = c->ar * 2 / w < 2 / h ? c->ar * 2 / w : 2 / h;
}

static inline void camera2DsetMaxSize(Camera2D *c, float w, float h) {
    w = fabsf(w);
    h = fabsf(h);
    c->zoom = c->ar * 2 / w > 2 / h ? c->ar * 2 / w : 2 / h;
}

static inline void camera2DsetMinRect(Camera2D *c, float x, float y, float w, float h) {
    c->center[0] = c->focus[0] = x + w / 2;
    c->center[1] = c->focus[1] = y + h / 2;
    camera2DsetMinSize(c, w, h);
}

static inline void camera2DsetMaxRect(Camera2D *c, float x, float y, float w, float h) {
    c->center[0] = c->focus[0] = x + w / 2;
    c->center[1] = c->focus[1] = y + h / 2;
    camera2DsetMaxSize(c, w, h);
}

static inline void camera2DgetRect(const Camera2D *c, float rect[4]) {
    rect[2] = camera2DgetWidth(c);
    rect[3] = camera2DgetHeight(c);
    rect[0] = c->focus[0] - rect[2] / 2;
    rect[1] = c->focus[1] - rect[3] / 2;
}

static inline void camera2Dmatrix(const Camera2D *c, float m[4][4]) {
    float a[4][4], b[4][4], v[4];
    float offset[4] = {c->center[0] - c->focus[0], c->center[1] - c->focus[1], 0, 0};

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
