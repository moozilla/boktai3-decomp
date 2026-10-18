#include "global.h"

extern u8 *gUnk_02000580[];
s32 Script_GetActorIndex(void);
s32 ScriptActor_GetDirectionFromScript(void);
s32 ScriptActor_GetCueOptionFromScript(void);
void ScriptActor_EnableCompletionScript(u8 *);
void PlayerCue_Kneel(u8 *, s32, s32);

void ScriptActor_KneelFromScript(void)
{
    u8 *p;
    s32 i = Script_GetActorIndex();
    s32 a;
    p = gUnk_02000580[i];
    if (p != 0) {
        a = ScriptActor_GetDirectionFromScript();
        PlayerCue_Kneel(p, a, ScriptActor_GetCueOptionFromScript());
        ScriptActor_EnableCompletionScript(p);
    }
}
