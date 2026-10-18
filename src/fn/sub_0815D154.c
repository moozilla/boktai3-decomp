#include "global.h"
s32 Script_GetActorIndex(void);
s32 ScriptActor_GetCueOptionFromScript(void);
s32 ScriptActor_GetDirectionFromScript(void);
void PlayerCue_Motion7Variant3(s32, s32, s32);
void ScriptActor_EnableCompletionScript(s32);
extern s32 gUnk_02000580[];
void ScriptActor_StartMotion7Variant3FromScript(void)
{
    s32 i = Script_GetActorIndex();
    s32 p = gUnk_02000580[i];
    s32 a;
    if (p != 0) {
        a = ScriptActor_GetDirectionFromScript();
        PlayerCue_Motion7Variant3(p, a, ScriptActor_GetCueOptionFromScript());
        ScriptActor_EnableCompletionScript(p);
    }
}
