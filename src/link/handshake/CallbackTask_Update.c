#include "global.h"
#include "engine/input.h"
#include "gba/io_reg.h"
#include "link/handshake/link_callback_task.h"
u8 CallbackTask_Finish(struct LinkCallbackTask *);

static inline void CallbackTask_Cancel(struct LinkCallbackTask *task)
{
    task->completed = 1;
    task->arg2 = 1;
    task->result = FALSE;
}

s32 CallbackTask_Update(struct LinkCallbackTask *task)
{
    if (CallbackTask_Finish(task))
        return 0;
    task->update(task);
    if (!task->cancellable || task->completed)
        return 0;
    if (!task->result && (gPadState[1] & B_BUTTON))
        CallbackTask_Cancel(task);
    return 0;
}
