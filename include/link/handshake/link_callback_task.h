#ifndef GUARD_INCLUDE_LINK_HANDSHAKE_LINK_CALLBACK_TASK_H
#define GUARD_INCLUDE_LINK_HANDSHAKE_LINK_CALLBACK_TASK_H

#include "global.h"

struct LinkCallbackTask {
    u8 pad0[0x1c];
    u32 cancellable;
    s32 arg2;
    s32 arg1;
    u32 result;
    u32 completed; /* confidence=low */
    u32 acknowledged;
    u8 pad34[0x3b];
    u8 arg3;
    u32 flags;
    void (*success)(void *, s32, s32, u32);
    void (*failure)(void *, s32, s32, u32);
    void (*update)(struct LinkCallbackTask *); /* confidence=low */
    void *context;
};

u8 CallbackTask_Finish(struct LinkCallbackTask *task);

typedef char LinkCallbackTaskSizeCheck[sizeof(struct LinkCallbackTask) == 0x84 ? 1 : -1];

#endif
