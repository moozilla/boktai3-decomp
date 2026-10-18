#include "global.h"
s32 Script_GetActorIndex(void);
s32 ScriptActor_GetCueOptionFromScript(void);
void PlayerCue_Motion30End(s32, s32);
void ScriptActor_EnableCompletionScript(s32);
extern s32 gUnk_02000580[];
void ScriptActor_EndMotion30FromScript(void)
{
    s32 i = Script_GetActorIndex();
    s32 p = gUnk_02000580[i];
    if (p != 0) {
        PlayerCue_Motion30End(p, ScriptActor_GetCueOptionFromScript());
        ScriptActor_EnableCompletionScript(p);
    }
}
