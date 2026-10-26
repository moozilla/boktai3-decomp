#include "script_keyframe_pool.h"
void ScriptKeyframe_UpdateEntry(struct ScriptKeyframeEntry *entry)
{
    if (entry->moving) {
        if (++entry->moveFrame >= entry->moveDuration) {
            entry->position = entry->target;
            entry->moving = 0;
        } else {
            s32 left = entry->moveDuration - entry->moveFrame;
            entry->position.x = Div(entry->target.x * entry->moveFrame + entry->origin.x * left,
                                    entry->moveDuration);
            entry->position.y = Div(entry->target.y * entry->moveFrame + entry->origin.y * left,
                                    entry->moveDuration);
            entry->position.z = Div(entry->target.z * entry->moveFrame + entry->origin.z * left,
                                    entry->moveDuration);
        }
        entry->animation.display.position = entry->position;
    }
    if (entry->conditionalVisibility) {
        if (!entry->hidden && gGameState->unk14)
            entry->animation.display.flags &= ~1;
        else
            entry->animation.display.flags |= 1;
    }
}
