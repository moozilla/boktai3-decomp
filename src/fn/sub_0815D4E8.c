#include "global.h"
s32 Script_GetActorIndex(void);
s32 ScriptActor_GetCueOptionFromScript(void);
s32 ScriptActor_GetDirectionFromScript(void);
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void ScriptActor_EnableCompletionScript(s32);
extern s32 gUnk_02000580[];
void PlayerCue_FaceSide(s32, s32);
void ScriptActor_FaceSideFromScript(void)
{
    s32 i = Script_GetActorIndex();
    s32 p = gUnk_02000580[i];
    s32 a;
    if (p != 0) {
        if (Script_SeekToKeyword(0x72) != 0)
            a = Script_GetValue();
        else
            a = 0;
        PlayerCue_FaceSide(p, a);
        ScriptActor_EnableCompletionScript(p);
    }
}
