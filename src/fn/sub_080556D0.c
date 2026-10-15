#include "script_keyframe_pool.h"
void ScriptKeyframe_SetFrameFromScript(void)
{
    struct ScriptKeyframeEntry *entry = ScriptKeyframe_FindFromScript();
    if (entry && Script_SeekToKeyword('p')) {
        union ScriptKeyframeAnimation *animation = &entry->animation;
        struct ProjectedDisplayResource *resource = &entry->resource;
        Anim_LoadKeyframe(animation, resource, (u16)Script_GetValue(), 0);
    }
}
