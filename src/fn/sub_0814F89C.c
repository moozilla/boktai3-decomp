#include "global.h"

void ScriptActor_CallCompletionScript(u8 *);
void PlayerCue_AcknowledgePendingTrigger(u8 *);

void PlayerCue_CompleteAction(u8 *p)
{
    u8 v = p[0x59E];

    if (v == 1) {
        ScriptActor_CallCompletionScript(p);
    } else if (v == 2) {
        PlayerCue_AcknowledgePendingTrigger(p);
    }
}
