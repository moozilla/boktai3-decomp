#include "global.h"

#include "engine.h"
#include "player.h"
s32 Script_GetActorIndex(void);
void PlayerCue_OverlaySet(u8 *, s32);
void ScriptActor_EnableCompletionScript(u8 *);
/* Script: on the scripted actor picked by -i, posts request 1 with id
   -p + 0x16E (+0xAE4/+0xAD6) and runs ScriptActor_EnableCompletionScript (tentative). */
void ScriptActor_RequestFromScript(void)
{
    s32 i = Script_GetActorIndex();
    u8 *actor = gPlayers[i];
    if (actor && Script_SeekToKeyword('p')) {
        PlayerCue_OverlaySet(actor, Script_GetValue() + 0x16E);
        ScriptActor_EnableCompletionScript(actor);
    }
}
