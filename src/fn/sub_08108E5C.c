#include "global.h"
#include "input.h"
// MATCHING_CONCESSION: register-pinning
struct CallbackTask {
    u8 pad0[0x1C];
    u32 cancellable;
    s32 arg2;
    s32 arg1;
    u32 result;
    u32 completed;
    u32 acknowledged;
    u8 pad34[0x6F - 0x34];
    u8 arg3;
    u32 flags;
    void (*success)(void *, s32, s32, u32);
    void (*failure)(void *, s32, s32, u32);
    void (*update)(struct CallbackTask *);
    void *context;
};
u8 CallbackTask_Finish(struct CallbackTask *);
s32 CallbackTask_Update(struct CallbackTask *task)
{
    if (CallbackTask_Finish(task))
        return 0;
    task->update(task);
    if (!task->cancellable)
        return 0;
    if (task->completed)
        return 0;
    {
        register u32 r asm("r2") = task->result;
        if (r)
            return 0;
    }
    if (gPadState[1] & 2) {
        task->completed = 1;
        task->arg2 = 1;
    }
    return 0;
}
