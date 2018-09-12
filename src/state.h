// TODO: add inAir counter and allow to jump if it's only a few ticks

#define STATE_DEFINE_ARRAY(type) \
    typedef struct { \
        size_t n; \
        type *arr; \
    } type##Array

STATE_DEFINE_ARRAY(CollRect);

#define STATE_COLOR_COUNT 9
enum {
    STATE_COLOR_BACKGROUND,
    STATE_COLOR_HERO,
    STATE_COLOR_WALL,
    STATE_COLOR_PASSIVE_EJECTOR,
    STATE_COLOR_ACTIVE_EJECTOR,
    STATE_COLOR_PULSATOR,
    STATE_COLOR_SHRINKER,
    STATE_COLOR_COIN,
    STATE_COLOR_GRAVITON,
};

#define STATE_EVENT_COUNT 9
enum {
    STATE_EVENT_ANTIBUMP,
    STATE_EVENT_BUMP,
    STATE_EVENT_JUMP,
    STATE_EVENT_EJECT,
    STATE_EVENT_COIN,
    STATE_EVENT_GRAVITON,
    STATE_EVENT_KEY,
    STATE_EVENT_DIE,
    STATE_EVENT_WIN
};

typedef struct {
    // TODO: move each variable to the array it belongs
    float tickDuration;
    float horVel, jumpVel, gravAcc, termVel;
    int ejectorCooldownTickCount;
    float ejectionVel;
    size_t pulsatorTableSize;
    float *pulsatorTable;
    int shrinkingTickCount;
    bool invertedGravity;
} StatePhysics;

typedef struct {
    CollRect r;
    float vVel;
    float envVelX, envVelY;
} StateHero;

typedef struct {
    int cooldown;
    CollRect r;
} StateEjector;
STATE_DEFINE_ARRAY(StateEjector);

typedef struct {
    int offset;
    CollRect r;
} StatePulsator;
STATE_DEFINE_ARRAY(StatePulsator);

typedef struct {
    int ticksLeft;
    CollRect r;
} StateShrinker;
STATE_DEFINE_ARRAY(StateShrinker);

typedef struct {
    bool taken;
    CollRect r;
} StatePickable;
STATE_DEFINE_ARRAY(StatePickable);

typedef struct {
    uint8_t keyColor[4];
    uint8_t lockColor[4];
    StatePickableArray key;
    int ticksLeft;
    CollRectArray lock;
} StateKey;
STATE_DEFINE_ARRAY(StateKey);

typedef struct {
    uint64_t tick;
    double lastTime;
    uint8_t color[STATE_COLOR_COUNT][4];
    int event[2][STATE_EVENT_COUNT];
    StatePhysics physics;
    BatchCall bg, fg;
    int winW, winH;
    Bmp lvl;
    StateHero hero;
    StateEjectorArray ejector;
    StatePulsatorArray pulsator;
    StateShrinkerArray shrinker;
    StatePickableArray coin, graviton;
    StateKeyArray key;
} State;

typedef struct {
    int winW, winH;
    float time;
    bool keyUp, keyLeft, keyRight, keyR;
} StateInput;

typedef enum {
    STATE_GAME_OVER_CAUSE_NONE,
    STATE_GAME_OVER_CAUSE_LOST,
    STATE_GAME_OVER_CAUSE_RESTART,
    STATE_GAME_OVER_CAUSE_WON
} StateGameOverCause;

State *stateNew(const char *path);
State *stateDel(State *state);
StateGameOverCause stateTick(State *state, const StateInput *in);
void stateDraw(State *state);
bool stateOpBumpCollision(const State *state, CollPen p);
StateGameOverCause stateOpGameOver(const State *state);
CollPen stateOpColl(const State *state, CollRect r);
void stateOpEnvEnergy(const State *state, float *velX, float *velY);
CollRect stateOpPulsator(const State *state, size_t i);
CollRect stateOpShrinker(const State *state, size_t i);
CollRect stateOpKeyLock(const State *state, size_t i, size_t j);
