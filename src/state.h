#define STATE_DEFINE_ARRAY(type) \
    typedef struct { \
        size_t n; \
        type *arr; \
    } type##Array

STATE_DEFINE_ARRAY(CollRect);

#define STATE_COLOR_COUNT 6
enum {
    STATE_COLOR_BACKGROUND,
    STATE_COLOR_HERO,
    STATE_COLOR_WALL,
    STATE_COLOR_PASSIVE_EJECTOR,
    STATE_COLOR_ACTIVE_EJECTOR,
    STATE_COLOR_COIN
};

typedef struct {
    float tickDuration;
    float horVel, jumpVel, gravAcc, termVel;
    int ejectorCooldownTickCount;
    float ejectionVel;
} StatePhysics;

typedef struct {
    CollRect r;
    float vVel;
} StateHero;

typedef struct {
    int cooldown;
    CollRect r;
} StateEjector;
STATE_DEFINE_ARRAY(StateEjector);

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
    BatchCall bg, fg;
    int winW, winH;
    Bmp lvl;
    StateHero hero;
    StateEjectorArray ejector;
    StatePickableArray coin;
} State;

typedef struct {
    int winW, winH;
    float time;
    bool keyUp, keyLeft, keyRight;
} StateInput;

State *stateNew(const char *path, const StateInput *in);
State *stateDel(State *state);
void stateTick(State *state, const StateInput *in);
void stateDraw(State *state);
bool stateOpBumpCollision(const State *state, CollPen p);
bool stateOpGameOver(const State *state);
