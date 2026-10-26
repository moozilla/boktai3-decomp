#include "script_keyframe_pool.h"
u32 ScriptKeyframe_TickManager(struct ScriptKeyframeManager *);
s32 ScriptKeyframe_DestroyManager(struct ScriptKeyframeManager *);
s32 ScriptKeyframe_InitManager(struct ScriptKeyframeManager *, u32, u32, u32);
struct ScriptKeyframeManager *ScriptKeyframe_CreateManagerFromScript(u32 arg1, u32 arg2)
{
    if (gScriptKeyframeManager)
        return gScriptKeyframeManager;
    {
        struct ScriptKeyframeManager *manager;
        u32 capacity;
        if (Script_SeekToKeyword('m'))
            capacity = Script_GetValue();
        else
            capacity = 1;
        manager = Task_Create(8, capacity * sizeof(struct ScriptKeyframeEntry) + 0x1C);
        if (manager) {
            Task_SetCallbacks(manager, (void (*)(void *))ScriptKeyframe_TickManager,
                              (void (*)(void *))ScriptKeyframe_DestroyManager);
            if (ScriptKeyframe_InitManager(manager, arg1, arg2, capacity) < 0) {
                Task_Kill(manager);
                return 0;
            }
        }
        return manager;
    }
}
