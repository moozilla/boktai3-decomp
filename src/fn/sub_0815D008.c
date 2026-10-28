#include "global.h"
#include "player.h"
s32 Script_GetActorIndex(void);
s32 ScriptActor_GetCueOptionFromScript(void);
s32 ScriptActor_GetDirectionFromScript(void);
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void ScriptActor_EnableCompletionScript(s32);
void PlayerCue_Motion30(s32, s32, s32, s32, s32);
void ScriptActor_PlayMotion30FromScript(void)
{
    s32 i = Script_GetActorIndex();
    s32 p = gPlayers[i];
    s32 a, b, c;
    if (p != 0) {
        if (Script_SeekToKeyword(0x66) != 0)
            b = Script_GetValue();
        else
            b = 0x32;
        if (Script_SeekToKeyword(0x4c) != 0)
            c = Script_GetValue();
        else
            c = 0;
        a = ScriptActor_GetDirectionFromScript();
        PlayerCue_Motion30(p, a, b, c, ScriptActor_GetCueOptionFromScript());
        ScriptActor_EnableCompletionScript(p);
    }
}
