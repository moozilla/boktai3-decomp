#include "global.h"

extern u8 *gUnk_02000580[];
s32 Script_GetActorIndex(void);
s32 ScriptActor_GetDirectionFromScript(void);
s32 ScriptActor_GetCueOptionFromScript(void);
void ScriptActor_EnableCompletionScript(u8 *);
void PlayerCue_Stand(u8 *, s32);

void ScriptActor_StandFromScript(void)
{
    u8 *p;
    s32 i = Script_GetActorIndex();
    p = gUnk_02000580[i];
    if (p != 0) {
        PlayerCue_Stand(p, ScriptActor_GetDirectionFromScript());
        ScriptActor_EnableCompletionScript(p);
    }
}
