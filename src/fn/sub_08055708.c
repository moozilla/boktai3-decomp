#include "script_keyframe_pool.h"
#define ScriptKeyframe_SetMirrorFromScript sub_08055708
void ScriptKeyframe_SetMirrorFromScript(void)
{
    struct ScriptKeyframeEntry *entry = sub_08055678();
    if (entry && Script_SeekToKeyword('f')) {
        if (Script_GetValue())
            entry->animation.display.flags |= 4;
        else
            entry->animation.display.flags &= ~4;
    }
}
