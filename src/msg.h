typedef enum {
    MSG_STARTED_TICK,
    MSG_ENDED_TICK,
    MSG_PICKED_COIN,
    MSG_PICKED_GRAVITON,
    MSG_PICKED_KEY,
    MSG_JUMPED,
    MSG_EJECTED,
    MSG_BUMP,
} MsgType;

typedef union {
    int dummy;
} MsgLoad;

typedef struct {
    MsgType type;
    MsgLoad load;
} Msg;

typedef void (*MsgFn)(const Msg *msg);

void msgSub(MsgFn fn);
void msgUsub(MsgFn fn);
void msgSend(Msg msg);
