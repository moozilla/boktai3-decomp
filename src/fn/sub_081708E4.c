#include "global.h"
void PauseMenu_CloseTextWindows(void *);
void MessageWindow_SetBounds(s32, s32, s32, s32);
void Hud_SetupBackgroundLayer(void *, u32, s32, s32, u32);
void Hud_DrawDefaultHealthMeter(void *, s32, s32, s32);
void Hud_DrawDefaultEnergyMeter(void *, s32, s32, s32, u32, u32);
void Hud_DrawHeaderCounter(void);
void PauseMenu_HideAllSprites(void *);
void PauseMenu_ClearSpriteFlags(void *, s32, s32);
void Anim_LoadKeyframe(void *, void *, u16, s32);
void sub_0816618C(void *, s32, s32);
void PauseMenu_LoadSpriteKeyframe(void *, s32, s32);
void sub_08166CD0(void *);
void sub_08166534(void *, s32, s32);
void sub_0817080C(void *);
void sub_08170718(void *);
void sub_081672FC(void *);
void sub_0816701C(void *, s32, s32, s32, s32, s32);
void ScrollingDialogue_RequestShow(void);
void sub_0816D868(void *);
void PauseMenu_SetPromptIconPositions(void *, s32, s32);
void sub_08166B7C(void *, s32);
void Hud_SetInteractionCallback(void *, void (*)(void), s32);
void sub_0816FCD0(void);
void PauseInventory_InitEquipmentPage(u8 *p)
{
    u8 *flag = p + 0x4860;
    u32 z = 0;
    u8 *o;
    u32 a, b;
    *flag = z;
    PauseMenu_CloseTextWindows(p);
    MessageWindow_SetBounds(2, 16, 28, 2);
    Hud_SetupBackgroundLayer(p, *(u32 *)(p + 0x18), 0, 6, *(u32 *)(p + 0x2c));
    Hud_DrawDefaultHealthMeter(p, 1, 2, 12);
    o = *(u8 **)(p + 0xa40);
    Hud_DrawDefaultEnergyMeter(p, 1, 2, 14, *(u16 *)(o + 0x428), *(u16 *)(o + 0x42a));
    Hud_DrawHeaderCounter();
    PauseMenu_HideAllSprites(p);
    PauseMenu_ClearSpriteFlags(p, 0, 0x201);
    {
        u8 *a = p + 0x1420;
        Anim_LoadKeyframe(a, p + 0x64, 0x52, 0);
    }
    sub_0816618C(p, 8, 32);
    PauseMenu_ClearSpriteFlags(p, 0x4d, 1);
    PauseMenu_ClearSpriteFlags(p, 8, 1);
    PauseMenu_LoadSpriteKeyframe(p, 8, 99);
    sub_08166CD0(p);
    sub_08166534(p, 10, 6);
    sub_0817080C(p);
    sub_08170718(p);
    sub_081672FC(p);
    sub_0816701C(p, 0, 0, 0, 1, z);
    ScrollingDialogue_RequestShow();
    sub_0816D868(p);
    PauseMenu_SetPromptIconPositions(p, 0, 0);
    sub_08166B7C(p, 4);
    Hud_SetInteractionCallback(p, sub_0816FCD0, 0);
}
