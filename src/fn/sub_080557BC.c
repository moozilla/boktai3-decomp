#include "script_keyframe_pool.h"
#define ScriptKeyframe_SetTargetFromScript sub_080557BC
void ScriptKeyframe_SetTargetFromScript(void)
{
    struct ScriptKeyframeEntry *entry = sub_08055678();
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
