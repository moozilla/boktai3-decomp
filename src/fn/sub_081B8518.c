#include "global.h"
struct SceneScriptTask {
    u8 unknown0[0x13C];
    u16 state;
};
struct SceneScriptCallbacks {
    void *entries[2];
};
extern struct SceneScriptCallbacks gUnk_0824F7E0;
s32 Callback_InvokeActor(void *, void *);
void SceneScriptTask_Update(struct SceneScriptTask *task)
{
    struct SceneScriptCallbacks callbacks = gUnk_0824F7E0;
    if (task->state <= 1)
        Callback_InvokeActor(task, callbacks.entries[task->state]);
}
