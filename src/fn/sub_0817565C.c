#include "global.h"
void PauseMenu_CloseTextWindows(u8 *);
void Hud_SetupBackgroundLayer(u8 *, u32, u32, u32, u32);
void PauseMenu_HideAllSprites(u8 *);
void PauseMenu_LoadSpriteKeyframe(u8 *, u32, u32);
void PauseMenu_ClearSpriteFlags(u8 *, u32, u32);
void sub_0816618C(u8 *, u32, u32);
void Hud_DrawDefaultHealthMeter(u8 *, u32, u32, u32);
void Hud_DrawDefaultEnergyMeter(u8 *, u32, u32, u32, u32, u32);
void Hud_DrawHeaderCounter(void);
void InventoryPanel_UpdateSelectedUnlock(u8 *, u32, u32, u32, u32);
void InventoryPanel_UpdateSelectedEquipment(u8 *, u32, u32, u32, u32);
void PlayerPanel_SnapshotValues(u8 *);
void InventoryPanel_ShowStatusIcons(u8 *);
void Hud_SetInteractionCallback(u8 *, void (*)(void), u32);
void PauseMenu_SetPromptIconPositions(u8 *, u32, s32);
void MenuDisplay_SetPaletteOverrides(u8 *, u32, u32, u32, u32);
void MessageWindow_Clear(void);
void ScrollingDialogue_ClearScript(void);
void ScrollingDialogue_RequestHide(void);
void TextWindow_Clear(u32);
void Presentation_InitListValues(u8 *);
void MenuSelection_DrawNumericSummary(u8 *);
void Hud_DrawCounter7B0(u32, u32);
void Hud_DrawCounter50C(u32, u32);
void Hud_DrawStatusDayNightIndicators(u8 *);
void sub_08174BBC(u8 *);
void SceneCatalog_SetSelection(u8 *, u32);
void InventoryGrid_LayoutStatMarkers(u8 *);
void SceneCatalog_ResetTextWindow(u8 *);
void SceneHud_SetPairedIcons(u8 *, u32, s32);
void SceneCatalog_SetDescription(u8 *, s32);
void InventoryGrid_UpdateModifierPreview(void);
void Presentation_UpdateAllocationSelection(void);
extern u16 gUnk_03004BD8;
void Hud_InitStatusMenu(u8 *s)
{
    u8 *p;
    s32 neg;
    u32 zero;
    PauseMenu_CloseTextWindows(s);
    MessageWindow_Clear();
    ScrollingDialogue_ClearScript();
    ScrollingDialogue_RequestHide();
    TextWindow_Clear(*(u32 *)(s + 0x48C4));
    TextWindow_Clear(*(u32 *)(s + 0x48C8));
    Hud_SetupBackgroundLayer(s, *(u32 *)(s + 0x18), 0, 9, *(u32 *)(s + 0x2C));
    PlayerPanel_SnapshotValues(s);
    Presentation_InitListValues(s);
    MenuSelection_DrawNumericSummary(s);
    Hud_DrawDefaultHealthMeter(s, 1, 8, 8);
    p = *(u8 **)(s + 0xA40);
    Hud_DrawDefaultEnergyMeter(s, 1, 8, 10, *(u16 *)(p + 0x428), *(u16 *)(p + 0x42A));
    Hud_DrawHeaderCounter();
    Hud_DrawCounter7B0(25, 9);
    Hud_DrawCounter50C(25, 10);
    Hud_DrawStatusDayNightIndicators(s);
    PauseMenu_HideAllSprites(s);
    PauseMenu_LoadSpriteKeyframe(s, 0, 0x57);
    PauseMenu_ClearSpriteFlags(s, 0x4D, 1);
    PauseMenu_ClearSpriteFlags(s, 0x4E, 1);
    sub_0816618C(s, 8, 0x20);
    sub_08174BBC(s);
    SceneCatalog_SetSelection(s, 0);
    InventoryGrid_LayoutStatMarkers(s);
    InventoryPanel_ShowStatusIcons(s);
    SceneCatalog_ResetTextWindow(s);
    PauseMenu_SetPromptIconPositions(s, 0, 8);
    neg = -1;
    SceneHud_SetPairedIcons(s, 1, neg);
    zero = 0;
    InventoryPanel_UpdateSelectedUnlock(s, 20, 7, 0, zero);
    InventoryPanel_UpdateSelectedEquipment(s, 26, 7, 0, zero);
    if (!*(u16 *)(s + 0x48A8)) {
        *(u32 *)(s + 0x1608) |= 1;
        s[0x48AB] = 0xFF;
        SceneCatalog_SetDescription(s, neg);
        Hud_SetInteractionCallback(s, InventoryGrid_UpdateModifierPreview, 0);
        MenuDisplay_SetPaletteOverrides(s, 0, 0, 0, zero);
    } else {
        u8 *obj = s + 0x3340;
        u32 n;
        *(u32 *)(obj + 8) &= ~1;
        *(u16 *)(s + 0x3360) = zero;
        *(u16 *)(s + 0x3362) = 8;
        n = s[0x48AA] + 20;
        s[0x48AB] = n;
        SceneCatalog_SetDescription(s, (s8)s[0x48AB]);
        Hud_SetInteractionCallback(s, Presentation_UpdateAllocationSelection, 0);
        MenuDisplay_SetPaletteOverrides(s, 1, 0, 0, zero);
    }
    {
        u16 *reg = &gUnk_03004BD8;
        s32 mask = ~0x200;
        mask &= *reg;
        *reg = mask;
    }
}
