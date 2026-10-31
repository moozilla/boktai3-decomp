#include "player_cue.h"

void ScriptActor_EnableCompletionScript(struct Player *p)
{
    PlayerCue_Messages(p)->completionKind = 1;
}
