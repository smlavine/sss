#define STATE_DEFINE_ARRAY(type) \
    typedef struct { \
        size_t n; \
        type *arr; \
    } type##Array

STATE_DEFINE_ARRAY(CollRect);

#define STATE_COLOR_COUNT 4
enum {
    STATE_COLOR_BACKGROUND,
    STATE_COLOR_HERO,
    STATE_COLOR_WALL,
    STATE_COLOR_COIN
};

typedef struct {
    float tickDuration;
    float horVel, jumpVel, gravAcc, termVel;
} StatePhysics;

typedef struct {
    CollRect r;
    float vVel;
} StateHero;

typedef struct {
    bool taken;
    CollRect r;
} StatePickable;
STATE_DEFINE_ARRAY(StatePickable);

typedef struct {
    uint64_t tick;
    double lastTime;
    uint8_t color[STATE_COLOR_COUNT][4];
    StatePhysics physics;
    int winW, winH;
    Bmp lvl;
    StateHero hero;
    BatchDrawCall bg, fg;
    StatePickableArray coin;
} State;

typedef struct {
    int winW, winH;
    float time;
    bool keyUp, keyLeft, keyRight;
} StateInput;

State *stateNew(const char *path, const StateInput *in);
State *stateDel(State *state);
void stateUpdate(State *state, const StateInput *in);
void stateDraw(State *state);
