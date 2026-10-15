#include "script_keyframe_pool.h"
void ScriptKeyframe_SetTargetFromScript(void)
{
    struct ScriptKeyframeEntry *entry = ScriptKeyframe_FindFromScript();
    if (entry) {
        if (Script_SeekToKeyword('p')) {
            entry->target.x = Script_GetValue();
            entry->target.y = Script_GetValue();
            entry->target.z = Script_GetValue();
        }
        if (Script_SeekToKeyword('f'))
            entry->moveDuration = Script_GetValue();
        entry->origin = entry->position;
        entry->moveFrame = 0;
        entry->moving = 1;
    }
}
