#include "global.h"
#define MenuPrompt_SetPhase         Hud_SetUpdateCallback
#define MenuPrompt_SetDisplayLevels Hud_InitSelectionBounds
struct MenuPrompt {
    u8 unk0[0xC4];
    u8 animData[0xA4C - 0xC4];
    u16 timer;
    u8 unkA4E[0x3BE0 - 0xA4E];
    u8 anim[0x48EA - 0x3BE0];
    u16 result;
    u8 unk48EC[4];
    u32 script;
};
s32 Anim_Step(void *, void *);
s32 MessageWindow_ShowMessage(s32);
int MessageWindow_SetNumberArgument(int, int);
void Script_Call(u32, void *);
void Hud_UpdateFirstPostScriptWait(struct MenuPrompt *);
void SceneHud_FadeOutPage(struct MenuPrompt *);
void Hud_SetUpdateCallback(struct MenuPrompt *, void (*)(struct MenuPrompt *));
void MenuPrompt_FinishDelayedChoice(struct MenuPrompt *p)
{
    Anim_Step(p->anim, p->animData);
    if (++p->timer > 150) {
        if (p->result) {
            MessageWindow_ShowMessage(5);
            if (p->script)
                Script_Call(p->script, 0);
            else
                MessageWindow_SetNumberArgument(0, 3);
            Hud_SetUpdateCallback(p, Hud_UpdateFirstPostScriptWait);
        } else {
            Hud_SetUpdateCallback(p, SceneHud_FadeOutPage);
        }
    }
}
