typedef struct {
    float x, y, w, h;
} Coll2Drect;

typedef struct {
    bool is;
    float south, north, west, east;
} Coll2Dpen;

typedef struct {
    float x, y, angle;
} Coll2Dray;

typedef struct {
    float x, y, angle, len;
} Coll2Dline;

Coll2Dpen coll2DbmpRect(const Bmp b, Coll2Drect r);
Coll2Dpen coll2Drect(Coll2Drect a, Coll2Drect b);
float coll2DrayLine(Coll2Dray r, Coll2Dline line);
float coll2DrayRect(Coll2Dray r, Coll2Drect rect);
