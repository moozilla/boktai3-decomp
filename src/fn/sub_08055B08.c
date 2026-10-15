#include "script_keyframe_pool.h"
#define ScriptKeyframe_CreateManagerFromScript sub_08055B08
u32 sub_08055A9C(struct ScriptKeyframeManager *);
s32 sub_08055AC4(struct ScriptKeyframeManager *);
s32 sub_08055AF4(struct ScriptKeyframeManager *, u32, u32, u32);
struct ScriptKeyframeManager *ScriptKeyframe_CreateManagerFromScript(u32 arg1, u32 arg2)
{
    if (gUnk_02000498)
        return gUnk_02000498;
    {
        struct ScriptKeyframeManager *manager;
        u32 capacity;
        if (Script_SeekToKeyword('m'))
            capacity = Script_GetValue();
        else
            capacity = 1;
        manager = Task_Create(8, capacity * sizeof(struct ScriptKeyframeEntry) + 0x1C);
        if (manager) {
            Task_SetCallbacks(manager, (void (*)(void *))sub_08055A9C,
                              (void (*)(void *))sub_08055AC4);
            if (sub_08055AF4(manager, arg1, arg2, capacity) < 0) {
                Task_Kill(manager);
                return 0;
            }
        }
        return manager;
    }
}
