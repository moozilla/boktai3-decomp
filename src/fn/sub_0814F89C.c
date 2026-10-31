#include "player_cue.h"

void ScriptActor_CallCompletionScript(struct Player *);
void PlayerCue_AcknowledgePendingTrigger(struct Player *);
void PlayerCue_CompleteAction(struct Player *p)
{
    u8 kind = PlayerCue_Messages(p)->completionKind;
    if (kind == 1)
        ScriptActor_CallCompletionScript(p);
    else if (kind == 2)
        PlayerCue_AcknowledgePendingTrigger(p);
}
