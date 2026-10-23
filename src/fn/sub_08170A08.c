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
void PlayerPanel_SnapshotValues(void *);
void sub_08166534(void *, s32, s32);
void SceneHud_InitializeActionInput(void *);
void SceneHud_LayoutActionGrid(void *);
void InventoryPanel_ShowStatusIcons(void *);
void InventoryPanel_DrawStatComparison(void *, s32, s32, s32, s32, s32);
void ScrollingDialogue_RequestShow(void);
void SceneHud_ShowActionPreview(void *);
void PauseMenu_SetPromptIconPositions(void *, s32, s32);
void MenuDisplay_UpdateActionPrompt(void *, s32);
void Hud_SetInteractionCallback(void *, void (*)(void), s32);
void PauseMenu_UpdateItemActions(void);
void MessageWindow_SetPalette(s32);
static inline void ShowFrame(u8 *obj, u8 *p, u16 frame, s32 mode)
{
    *(u32 *)(obj + 8) &= ~1;
    Anim_LoadKeyframe(obj, p + 0x64, frame, mode);
}
void sub_08170A08(u8 *p)
{
    u8 *flag;
    u32 z, one;
    u8 *o;
    PauseMenu_CloseTextWindows(p);
    flag = p + 0x4860;
    z = 0;
    one = 1;
    *flag = one;
    MessageWindow_SetBounds(2, 16, 28, 2);
    Hud_SetupBackgroundLayer(p, *(u32 *)(p + 0x18), 0, 6, *(u32 *)(p + 0x2c));
    Hud_DrawDefaultHealthMeter(p, 1, 2, 12);
    o = *(u8 **)(p + 0xa40);
    Hud_DrawDefaultEnergyMeter(p, 1, 2, 14, *(u16 *)(o + 0x428), *(u16 *)(o + 0x42a));
    Hud_DrawHeaderCounter();
    PauseMenu_HideAllSprites(p);
    ShowFrame(p + 0x1420, p, 0x53, 0);
    sub_0816618C(p, 8, 32);
    PauseMenu_ClearSpriteFlags(p, 0x4d, 1);
    ShowFrame(p + 0x1720, p, 0x64, 1);
    PlayerPanel_SnapshotValues(p);
    sub_08166534(p, 10, 6);
    SceneHud_InitializeActionInput(p);
    SceneHud_LayoutActionGrid(p);
    InventoryPanel_ShowStatusIcons(p);
    InventoryPanel_DrawStatComparison(p, 0, 0, 0, one, z);
    ScrollingDialogue_RequestShow();
    MessageWindow_SetPalette(0xEFDA);
    SceneHud_ShowActionPreview(p);
    PauseMenu_SetPromptIconPositions(p, 0, 0);
    MenuDisplay_UpdateActionPrompt(p, 4);
    Hud_SetInteractionCallback(p, PauseMenu_UpdateItemActions, 0);
}
