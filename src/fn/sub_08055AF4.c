#include "script_keyframe_pool.h"
s32 ScriptKeyframe_InitManager(struct ScriptKeyframeManager *manager, u32 unused1, u32 unused2,
                               u32 capacity)
{
    u32 zero = 0;
    manager->capacity = capacity;
    manager->count = zero;
    gScriptKeyframeManager = manager;
    return 0;
}
