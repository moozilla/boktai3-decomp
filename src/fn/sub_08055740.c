#include "script_keyframe_pool.h"
#define ScriptKeyframe_HideFromScript sub_08055740
void ScriptKeyframe_HideFromScript(void)
{
    struct ScriptKeyframeEntry *entry = sub_08055678();
    if (entry) {
        u32 flags = entry->animation.display.flags;
        u32 mask = 1;
        flags |= mask;
        entry->animation.display.flags = flags;
        if (entry->conditionalVisibility)
            entry->hidden = mask;
    }
}
