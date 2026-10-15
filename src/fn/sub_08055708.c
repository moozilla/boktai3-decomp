#include "script_keyframe_pool.h"
void ScriptKeyframe_SetMirrorFromScript(void)
{
    struct ScriptKeyframeEntry *entry = ScriptKeyframe_FindFromScript();
    if (entry && Script_SeekToKeyword('f')) {
        if (Script_GetValue())
            entry->animation.display.flags |= 4;
        else
            entry->animation.display.flags &= ~4;
    }
}
