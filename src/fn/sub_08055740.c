#include "script_keyframe_pool.h"
void ScriptKeyframe_HideFromScript(void)
{
    struct ScriptKeyframeEntry *entry = ScriptKeyframe_FindFromScript();
    if (entry) {
        u32 flags = entry->animation.display.flags;
        u32 mask = 1;
        flags |= mask;
        entry->animation.display.flags = flags;
        if (entry->conditionalVisibility)
            entry->hidden = mask;
    }
}
