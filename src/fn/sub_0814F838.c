#include "player_cue.h"

void ScriptActor_CallCompletionScript(struct Player *p)
{
    u32 *slot = &PlayerCue_Messages(p)->completionScript;
    u32 script = *slot;
    if (script != 0) {
        *slot = 0;
        Script_Call(script, 0);
    }
}
