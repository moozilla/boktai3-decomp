#include "script_keyframe_pool.h"
s32 ScriptKeyframe_DestroyManager(struct ScriptKeyframeManager *manager)
{
    s32 i = 0;
    if (i < manager->count) {
        union ScriptKeyframeAnimation *animation = &manager->entries[0].animation;
        do {
            Anim_Remove(animation);
            animation = (void *)((u8 *)animation + sizeof(struct ScriptKeyframeEntry));
            i++;
        } while (i < manager->count);
    }
    gScriptKeyframeManager = 0;
    return 0;
}
