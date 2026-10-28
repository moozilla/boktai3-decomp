#include "global.h"
#include "player.h"

s32 Script_GetActorIndex(void);
s32 ScriptActor_GetDirectionFromScript(void);
s32 ScriptActor_GetCueOptionFromScript(void);
void ScriptActor_EnableCompletionScript(u8 *);
void PlayerCue_StandDiagonalB(u8 *, s32, s32);

void ScriptActor_StandDiagonalBFromScript(void)
{
    u8 *p;
    s32 i = Script_GetActorIndex();
    s32 a;
    p = gPlayers[i];
    if (p != 0) {
        a = ScriptActor_GetDirectionFromScript();
        PlayerCue_StandDiagonalB(p, a, ScriptActor_GetCueOptionFromScript());
        ScriptActor_EnableCompletionScript(p);
    }
}
