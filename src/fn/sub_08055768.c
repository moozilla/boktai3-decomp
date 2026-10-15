#include "script_keyframe_pool.h"
#define ScriptKeyframe_ShowFromScript sub_08055768
void ScriptKeyframe_ShowFromScript(void)
{
    struct ScriptKeyframeEntry *entry = sub_08055678();
    if (entry) {
        entry->animation.display.flags &= ~1;
        if (entry->conditionalVisibility)
            entry->hidden = 0;
    }
}
