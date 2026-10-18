#include "global.h"
s32 Script_GetActorIndex(void);
s32 ScriptActor_GetCueOptionFromScript(void);
s32 ScriptActor_GetDirectionFromScript(void);
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void ScriptActor_EnableCompletionScript(s32);
extern s32 gUnk_02000580[];
void sub_0815B774(s32, s32);
void ScriptActor_StartMotionB774FromScript(void)
{
    s32 i = Script_GetActorIndex();
    s32 p = gUnk_02000580[i];
    if (p != 0) {
        sub_0815B774(p, ScriptActor_GetDirectionFromScript());
        ScriptActor_EnableCompletionScript(p);
    }
}
