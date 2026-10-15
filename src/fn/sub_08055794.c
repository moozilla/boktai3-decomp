#include "script_keyframe_pool.h"
#define ScriptKeyframe_SetPriorityFromScript sub_08055794
void ScriptKeyframe_SetPriorityFromScript(void)
{
    struct ScriptKeyframeEntry *entry = sub_08055678();
    if (entry && Script_SeekToKeyword('p'))
        entry->animation.keyframe.priority = Script_GetValue();
}
