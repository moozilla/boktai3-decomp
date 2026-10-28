#include "global.h"
#include "player.h"
s32 Script_GetActorIndex(void);
s32 ScriptActor_GetCueOptionFromScript(void);
s32 ScriptActor_GetDirectionFromScript(void);
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void ScriptActor_EnableCompletionScript(s32);
void PlayerCue_Motion29(s32, s32, s32, s32);
void ScriptActor_PlayMotion29FromScript(void)
{
    s32 i = Script_GetActorIndex();
    s32 p = gPlayers[i];
    s32 a, b;
    if (p != 0) {
        if (Script_SeekToKeyword(0x66) != 0)
            b = Script_GetValue();
        else
            b = 0x1e;
        a = ScriptActor_GetDirectionFromScript();
        PlayerCue_Motion29(p, a, b, ScriptActor_GetCueOptionFromScript());
        ScriptActor_EnableCompletionScript(p);
    }
}
