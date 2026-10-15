#include "script_keyframe_pool.h"
#define ScriptKeyframe_SetFrameFromScript sub_080556D0
void ScriptKeyframe_SetFrameFromScript(void)
{
    struct ScriptKeyframeEntry *entry = sub_08055678();
    if (entry && Script_SeekToKeyword('p')) {
        union ScriptKeyframeAnimation *animation = &entry->animation;
        struct ProjectedDisplayResource *resource = &entry->resource;
        Anim_LoadKeyframe(animation, resource, (u16)Script_GetValue(), 0);
    }
}
