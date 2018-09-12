#include <stdlib.h>

#include "msg.h"

static struct {
    size_t size;
    MsgFn *elem;
} subs = {0, NULL};

void msgSub(MsgFn fn) {
    subs.elem = realloc(subs.elem, ++subs.size * sizeof(*subs.elem));
    subs.elem[subs.size - 1] = fn;
}

void msgUsub(MsgFn fn) {
    for (size_t i = 0; i < subs.size; ++i) {
        if (subs.elem[i] == fn) {
            for (size_t j = i + 1; j < subs.size; ++j) {
                subs.elem[j] = subs.elem[i];
            }
            subs.elem = realloc(subs.elem, --subs.size * sizeof(*subs.elem));
            return;
        }
    }
}

void msgSend(Msg msg) {
    for (size_t i = 0; i < subs.size; ++i) {
        subs.elem[i](&msg);
    }
}
