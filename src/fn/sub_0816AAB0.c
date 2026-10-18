#include "global.h"
struct MenuPrompt {
    u8 unk0[0x48F4];
    void *child;
};
s32 sub_081BD914(void *);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void ScenePanel_BeginAccountMenu(struct MenuPrompt *);
void MenuPrompt_WaitForChildPhase(struct MenuPrompt *p)
{
    if (sub_081BD914(p->child)) {
        ScreenTransition_SendCommand(2, 4, 4, 4, 4, 65535, 0);
        ScenePanel_BeginAccountMenu(p);
    }
}
