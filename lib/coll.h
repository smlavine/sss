typedef struct {
    float x, y, w, h;
} CollRect;

typedef struct {
    bool is;
    float south, north, west, east;
} CollPen;

typedef struct {
    float x, y, angle;
} CollRay;

typedef struct {
    float x, y, angle, len;
} CollLine;

CollPen collBmpRect(const Bmp b, CollRect r);
CollPen collRect(CollRect a, CollRect b);
float collRayLine(CollRay r, CollLine line);
float collRayRect(CollRay r, CollRect rect);
