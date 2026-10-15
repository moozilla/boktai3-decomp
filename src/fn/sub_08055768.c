#include "script_keyframe_pool.h"
void ScriptKeyframe_ShowFromScript(void)
{
    struct ScriptKeyframeEntry *entry = ScriptKeyframe_FindFromScript();
    if (entry) {
        entry->animation.display.flags &= ~1;
        if (entry->conditionalVisibility)
            entry->hidden = 0;
    }
}
