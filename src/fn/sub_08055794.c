#include "script_keyframe_pool.h"
void ScriptKeyframe_SetPriorityFromScript(void)
{
    struct ScriptKeyframeEntry *entry = ScriptKeyframe_FindFromScript();
    if (entry && Script_SeekToKeyword('p'))
        entry->animation.keyframe.priority = Script_GetValue();
}
