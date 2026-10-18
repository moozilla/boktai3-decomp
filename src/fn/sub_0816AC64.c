#include "global.h"
void Hud_SetupBackgroundLayer(u8 *, u32, u32, u32, u32);
void PauseMenu_HideAllSprites(u8 *);
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
void Anim_LoadKeyframe(void *, void *, u32, u32);
void Hud_SetUpdateCallback(u8 *, void *);
void sub_0816AF5C(void);
void Hud_InitRetryPanel(u8 *s)
{
    u32 zero;
    u16 *p;
    u8 *a, *b, *initial;

    MessageWindow_Clear();
    ScrollingDialogue_ClearScript();
    ScrollingDialogue_RequestHide();
    MessageWindow_SetBounds(1, 1, 28, 6);
    MessageWindow_SetMessageTable(*(u32 *)(s + 0x58));
    MessageWindow_SetDisplayMode(1);
    Hud_SetupBackgroundLayer(s, *(u32 *)(s + 0x18), 0, 2, *(u32 *)(s + 0x2C));
    PauseMenu_HideAllSprites(s);
    *(u32 *)(s + 0x3BE8) &= ~1;
    p = (u16 *)(s + 0x3C00);
    zero = 0;
    {
        u32 v = 0xFFA8;
        *p = v;
    }
    *(u16 *)(s + 0x3C02) = 0x20;
    initial = s + 0x3BE0;
    b = s + 0xC4;
    Anim_SetKeyframes(initial, b, 4, 2, zero);
    a = s + 0x3D00;
    Anim_LoadKeyframe(a, b, 0x1D, 1);
    Anim_SetKeyframes(a, b, 10, 1, zero);
    MessageWindow_ShowMessage(0);
    Hud_SetUpdateCallback(s, sub_0816AF5C);
}
