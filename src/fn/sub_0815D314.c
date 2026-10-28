#include "global.h"
#include "player.h"
s32 Script_GetActorIndex(void);
s32 ScriptActor_GetCueOptionFromScript(void);
s32 ScriptActor_GetDirectionFromScript(void);
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void ScriptActor_EnableCompletionScript(s32);
void PlayerCue_Shoot(s32, s32, s32);
void ScriptActor_ShootFromScript(void)
{
    s32 i = Script_GetActorIndex();
    s32 p = gPlayers[i];
    s32 a;
    if (p != 0) {
        if (Script_SeekToKeyword(0x6c) != 0)
            a = Script_GetValue();
        else
            a = 0x20;
        PlayerCue_Shoot(p, a, ScriptActor_GetCueOptionFromScript());
        ScriptActor_EnableCompletionScript(p);
    }
}
