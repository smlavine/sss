#define STATE_DEFINE_ARRAY(type) \
    typedef struct { \
        size_t n; \
        type *arr; \
    } type##Array

typedef struct {
    int w, h;
} StateLvl;

typedef struct {
    int x, y, w, h;
} StateRect;
STATE_DEFINE_ARRAY(StateRect);

#define STATE_COLOR_COUNT 4
enum {
    STATE_COLOR_BACKGROUND,
    STATE_COLOR_HERO,
    STATE_COLOR_WALL,
    STATE_COLOR_COIN
};

typedef struct {
    uint8_t color[STATE_COLOR_COUNT][4];
    int winW, winH;
    StateLvl lvl;
    StateRect hero;
    BatchDrawCall bg, fg;
    StateRectArray coin;
} State;

typedef struct {
    int winW, winH;
} StateInput;

State *stateNew(const char *path, const StateInput *in);
State *stateDel(State *state);
void stateUpdate(State *state, const StateInput *in);
void stateDraw(State *state);
