#include "global.h"
#include "player.h"
s32 Script_GetActorIndex(void);
s32 ScriptActor_GetCueOptionFromScript(void);
s32 ScriptActor_GetDirectionFromScript(void);
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void ScriptActor_EnableCompletionScript(s32);
s32 PlayerCue_FormMotion(s32, s32);
void ScriptActor_CallCompletionScript(s32);
void ScriptActor_StartFormMotionFromScript(void)
{
    s32 i = Script_GetActorIndex();
    s32 p = gPlayers[i];
    s32 r;
    if (p != 0) {
        r = PlayerCue_FormMotion(p, ScriptActor_GetCueOptionFromScript());
        ScriptActor_EnableCompletionScript(p);
        if (r == 0)
            ScriptActor_CallCompletionScript(p);
    }
}
