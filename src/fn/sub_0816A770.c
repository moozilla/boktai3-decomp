#include "global.h"
void PauseMenu_HideAllSprites(u8 *);
void Hud_SetupBackgroundLayer(u8 *, u32, s32, s32, u32);
void Anim_SetKeyframes(void *, void *, s32, s32, s32);
void MessageWindow_ShowMessage(s32);
void Hud_SetUpdateCallback(u8 *, void (*)(u8 *));
void MenuPrompt_PulseDisplay(u8 *);
void sub_0816A770(u8 *hud)
{
    u16 zero;
    PauseMenu_HideAllSprites(hud);
    Hud_SetupBackgroundLayer(hud, *(u32 *)(hud + 0x24), 0, 5, *(u32 *)(hud + 0x34));
    *(u32 *)(hud + 0x3be8) &= ~1;
    *(u16 *)(hud + 0x3c00) = zero = 0;
    *(u16 *)(hud + 0x3c02) = zero;
    Anim_SetKeyframes(hud + 0x3be0, hud + 0xc4, 6, 2, zero);
    MessageWindow_ShowMessage(13);
    Hud_SetUpdateCallback(hud, MenuPrompt_PulseDisplay);
}
