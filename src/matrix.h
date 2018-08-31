static inline void matrixTranslation(float matrix[4][4], float x, float y, float z) {
    matrix[0][0] = 1;
    matrix[1][0] = 0;
    matrix[2][0] = 0;
    matrix[3][0] = x;

    matrix[0][1] = 0;
    matrix[1][1] = 1;
    matrix[2][1] = 0;
    matrix[3][1] = y;

    matrix[0][2] = 0;
    matrix[1][2] = 0;
    matrix[2][2] = 1;
    matrix[3][2] = z;

    matrix[0][3] = 0;
    matrix[1][3] = 0;
    matrix[2][3] = 0;
    matrix[3][3] = 1;
}

static inline void matrixScale(float matrix[4][4], float x, float y, float z) {
    matrix[0][0] = x;
    matrix[1][0] = 0;
    matrix[2][0] = 0;
    matrix[3][0] = 0;

    matrix[0][1] = 0;
    matrix[1][1] = y;
    matrix[2][1] = 0;
    matrix[3][1] = 0;

    matrix[0][2] = 0;
    matrix[1][2] = 0;
    matrix[2][2] = z;
    matrix[3][2] = 0;

    matrix[0][3] = 0;
    matrix[1][3] = 0;
    matrix[2][3] = 0;
    matrix[3][3] = 1;
}

static inline void matrixRotationZ(float matrix[4][4], float angle) {
    float s = sinf(angle);
    float c = cosf(angle);

    matrix[0][0] = c;
    matrix[1][0] = s;
    matrix[2][0] = 0;
    matrix[3][0] = 0;

    matrix[0][1] = -s;
    matrix[1][1] = c;
    matrix[2][1] = 0;
    matrix[3][1] = 0;

    matrix[0][2] = 0;
    matrix[1][2] = 0;
    matrix[2][2] = 1;
    matrix[3][2] = 0;

    matrix[0][3] = 0;
    matrix[1][3] = 0;
    matrix[2][3] = 0;
    matrix[3][3] = 1;
}

static inline void matrixProduct(float ab[4][4], const float a[4][4], const float b[4][4]) {
    ab[0][0] = a[0][0] * b[0][0] + a[1][0] * b[0][1] + a[2][0] * b[0][2] + a[3][0] * b[0][3];
    ab[1][0] = a[0][0] * b[1][0] + a[1][0] * b[1][1] + a[2][0] * b[1][2] + a[3][0] * b[1][3];
    ab[2][0] = a[0][0] * b[2][0] + a[1][0] * b[2][1] + a[2][0] * b[2][2] + a[3][0] * b[2][3];
    ab[3][0] = a[0][0] * b[3][0] + a[1][0] * b[3][1] + a[2][0] * b[3][2] + a[3][0] * b[3][3];

    ab[0][1] = a[0][1] * b[0][0] + a[1][1] * b[0][1] + a[2][1] * b[0][2] + a[3][1] * b[0][3];
    ab[1][1] = a[0][1] * b[1][0] + a[1][1] * b[1][1] + a[2][1] * b[1][2] + a[3][1] * b[1][3];
    ab[2][1] = a[0][1] * b[2][0] + a[1][1] * b[2][1] + a[2][1] * b[2][2] + a[3][1] * b[2][3];
    ab[3][1] = a[0][1] * b[3][0] + a[1][1] * b[3][1] + a[2][1] * b[3][2] + a[3][1] * b[3][3];

    ab[0][2] = a[0][2] * b[0][0] + a[1][2] * b[0][1] + a[2][2] * b[0][2] + a[3][2] * b[0][3];
    ab[1][2] = a[0][2] * b[1][0] + a[1][2] * b[1][1] + a[2][2] * b[1][2] + a[3][2] * b[1][3];
    ab[2][2] = a[0][2] * b[2][0] + a[1][2] * b[2][1] + a[2][2] * b[2][2] + a[3][2] * b[2][3];
    ab[3][2] = a[0][2] * b[3][0] + a[1][2] * b[3][1] + a[2][2] * b[3][2] + a[3][2] * b[3][3];

    ab[0][3] = a[0][3] * b[0][0] + a[1][3] * b[0][1] + a[2][3] * b[0][2] + a[3][3] * b[0][3];
    ab[1][3] = a[0][3] * b[1][0] + a[1][3] * b[1][1] + a[2][3] * b[1][2] + a[3][3] * b[1][3];
    ab[2][3] = a[0][3] * b[2][0] + a[1][3] * b[2][1] + a[2][3] * b[2][2] + a[3][3] * b[2][3];
    ab[3][3] = a[0][3] * b[3][0] + a[1][3] * b[3][1] + a[2][3] * b[3][2] + a[3][3] * b[3][3];
}

static inline void matrixVectorProduct(float v[4], const float m[4][4], const float x[4]) {
    v[0] = m[0][0] * x[0] + m[1][0] * x[1] + m[2][0] * x[2] + m[3][0] * x[3];
    v[1] = m[0][1] * x[0] + m[1][1] * x[1] + m[2][1] * x[2] + m[3][1] * x[3];
    v[2] = m[0][2] * x[0] + m[1][2] * x[1] + m[2][2] * x[2] + m[3][2] * x[3];
    v[3] = m[0][3] * x[0] + m[1][3] * x[1] + m[2][3] * x[2] + m[3][3] * x[3];
}
