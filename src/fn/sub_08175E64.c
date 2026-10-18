#include "global.h"
struct PauseMenuConfig {
    u8 unknown0[0x64];
    u8 animationData[0x159C];
    u8 animation[0x20];
    u16 choiceX, choiceY, choiceState;
    u8 unknown1626[0x10C2];
    u32 optionFlags0;
    u8 unknown26EC[0x5C];
    u32 optionFlags1;
    u8 unknown274C[0x5C];
    u32 optionFlags2;
};
void SelectionPanel_CaptureSettings(struct PauseMenuConfig *);
void sub_0817597C(struct PauseMenuConfig *, u32);
s32 Anim_LoadKeyframe(void *, void *, u32, u32);
void MenuDisplay_SetPaletteOverrides(struct PauseMenuConfig *, s32, s32, s32, s32);
void PauseMenu_InitConfigChoice(struct PauseMenuConfig *s)
{
    SelectionPanel_CaptureSettings(s);
    s->optionFlags0 |= 1;
    s->optionFlags1 |= 1;
    s->optionFlags2 |= 1;
    Anim_LoadKeyframe(s->animation, s->animationData, 0x47, 0);
    s->choiceX = 0;
    s->choiceY = 0x18;
    s->choiceState = 0;
    MenuDisplay_SetPaletteOverrides(s, 1, 0, 0, 0);
    sub_0817597C(s, 0);
}
