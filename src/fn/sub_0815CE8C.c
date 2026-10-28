#include "global.h"
#include "player.h"

s32 Script_GetActorIndex(void);
s32 ScriptActor_GetDirectionFromScript(void);
s32 ScriptActor_GetCueOptionFromScript(void);
void ScriptActor_EnableCompletionScript(u8 *);
void PlayerCue_KneelGasp(u8 *, s32);

void ScriptActor_KneelGaspFromScript(void)
{
    u8 *p;
    s32 i = Script_GetActorIndex();
    p = gPlayers[i];
    if (p != 0) {
        PlayerCue_KneelGasp(p, ScriptActor_GetDirectionFromScript());
        ScriptActor_EnableCompletionScript(p);
    }
}
