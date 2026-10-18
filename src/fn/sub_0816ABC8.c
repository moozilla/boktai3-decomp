#include "global.h"
#define MenuPrompt_SetPhase Hud_SetUpdateCallback
struct MenuPrompt {
    u8 unk0[0x48F4];
    void *child;
};
s32 MessageWindow_Clear(void);
u32 ScrollingDialogue_ClearScript(void);
void ScrollingDialogue_RequestHide(void);
s32 sub_081BD914(void *);
void PauseMenu_HideAllSprites(void *);
void DarkLoan_OpenDisplay(void *);
void MenuPrompt_WaitForChildPhase(struct MenuPrompt *);
void Hud_SetUpdateCallback(struct MenuPrompt *, void (*)(struct MenuPrompt *));
void MenuPrompt_ResumeChild(struct MenuPrompt *p)
{
    MessageWindow_Clear();
    ScrollingDialogue_ClearScript();
    ScrollingDialogue_RequestHide();
    if (sub_081BD914(p->child)) {
        PauseMenu_HideAllSprites(p);
        DarkLoan_OpenDisplay(p->child);
        Hud_SetUpdateCallback(p, MenuPrompt_WaitForChildPhase);
    }
}
