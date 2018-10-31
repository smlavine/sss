#define S_SHRINKING_TICKS 20

typedef struct {
    AudioMusic *music;
    AudioSound *coin, *graviton, *key, *jump, *eject, *die, *win;
} Saudio;

extern struct S {

    struct {
        Batch bg, fg;
    } draw;

    struct {
        long long tick;
        float lastTime;
    } tick;

    Bmp lvl;

    struct {
        CollRect r;
        float vVel, envVelX, envVelY;
    } hero;

    struct {
        size_t n;
        struct {
            int cooldown;
            CollRect r;
        } *arr;
    } ejector;

    struct {
        size_t n;
        struct {
            long long offset;
            CollRect r;
        } *arr;
    } pulsator;

    struct {
        size_t n;
        struct {
            long long ticksLeft;
            CollRect r;
        } *arr;
    } shrinker;

    struct {
        size_t n;
        struct {
            bool taken;
            CollRect r;
        } *arr;
    } coin;

    struct {
        bool invertedGravity;
        size_t n;
        struct {
            bool taken;
            CollRect r;
        } *arr;
    } graviton;

    struct {
        size_t n;
        struct {
            long long ticksLeft;
            size_t n;
            struct {
                bool taken;
                CollRect r;
            } *arr;
            struct {
                size_t n;
                CollRect *arr;
            } lock, antilock;
        } *arr;
    } key;

    struct {
        size_t n;
        struct {
            float w, h;
            size_t n;
            struct {
                float x, y;
            } *arr;
        } *arr;
    } plat;

} s; // (data.c)

// data.c
void sLoad(const char *path);
void sFree(void);

// tick.c
int sTick(Saudio audio,double t,bool kUp,bool kLeft,bool kRight,bool kR);

// draw.c
void sDraw(int winW, int winH);

// op.c
CollRect sOpPulsator(size_t i);
CollRect sOpShrinker(size_t i);
CollRect sOpKeyLock(size_t i, size_t j);
CollRect sOpKeyAntilock(size_t i, size_t j);
CollRect sOpPlat(size_t i);
CollPen sOpColl(const CollRect r);
int sOpGameOver(void);
bool sOpBumpCollision(const CollPen p);
void sOpEnvEnergy(float *velX, float *velY);
