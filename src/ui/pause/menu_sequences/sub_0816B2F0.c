#define SceneSequence_SetPhase Hud_SetUpdateCallback
#include "global.h"
struct SceneSequence {
    u8 unknown0[0xC4];
    u8 animationData[0xA4C - 0xC4];
    u16 phaseTimer;
    u8 unknownA4E[0x3BE0 - 0xA4E];
    u8 animation[0x48F0 - 0x3BE0];
    u32 completionScript;
};
s32 Anim_Step(void *, void *);
s32 MessageWindow_ShowMessage(s32);
void Script_Call(u32, void *);
int MessageWindow_SetNumberArgument(int, int);
void Hud_SetUpdateCallback(void *, void (*)(void));
void Hud_UpdateSecondPostScriptWait(void);
void SceneSequence_CallCompletionScript(struct SceneSequence *s)
{
    Anim_Step(s->animation, s->animationData);
    if (++s->phaseTimer > 149) {
        MessageWindow_ShowMessage(5);
        if (s->completionScript)
            Script_Call(s->completionScript, 0);
        else
            MessageWindow_SetNumberArgument(0, 3);
        Hud_SetUpdateCallback(s, Hud_UpdateSecondPostScriptWait);
    }
}
