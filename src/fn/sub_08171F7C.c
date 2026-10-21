#include "global.h"
/* Tentative: builds the consumable action screen of the pause inventory. */
void PauseMenu_CloseTextWindows(u8 *);
void Hud_SetupBackgroundLayer(u8 *, u32, u32, u32, u32);
void PauseMenu_HideAllSprites(void *);
void Hud_DrawHeaderCounter(void);
s32 Anim_LoadKeyframe(void *, void *, u32, u32);
void PauseMenu_ClearSpriteFlags(void *, s32, u32);
void sub_0816618C(void *, s32, s32);
void Hud_DrawDefaultResourceCounts(s32, s32, s32, s32, s32);
void InventoryPanel_UpdateSelectedUnlock(void *, int, int, int, int);
void InventoryPanel_UpdateSelectedEquipment(void *, int, int, int, int);
void PlayerPanel_SnapshotValues(void *);
void sub_08166534(void *, s32, s32);
void InventoryPanel_ShowStatusIcons(void *);
void PauseInventory_InitConsumableCursors(void *);
void SceneHud_LayoutAlternateActionGrid(void *);
void sub_08170D78(void *);
void PauseMenu_SetPromptIconPositions(u8 *, u32, s32);
void MenuDisplay_UpdateActionPrompt(void *, int);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void MenuDisplay_SetPaletteOverrides(void *, int, int, int, int);
void ScrollingDialogue_HideImmediately(void);
void PauseInventory_UpdateConsumableActions(void);
struct HudObj {
    u8 unknown0[8];
    u32 flags;
};
void sub_08171F7C(u8 *p)
{
    PauseMenu_CloseTextWindows(p);
    Hud_SetupBackgroundLayer(p, *(u32 *)(p + 0x18), 0, 8, *(u32 *)(p + 0x2C));
    PauseMenu_HideAllSprites(p);
    Hud_DrawHeaderCounter();
    ((struct HudObj *)(p + 0x1420))->flags &= ~1;
    Anim_LoadKeyframe(p + 0x1420, p + 0x64, 0x56, 0);
    PauseMenu_ClearSpriteFlags(p, 0x4D, 1);
    sub_0816618C(p, 8, 0x20);
    Hud_DrawDefaultResourceCounts(8, 4, 8, 3, 0);
    InventoryPanel_UpdateSelectedUnlock(p, 8, 3, 0, 0);
    InventoryPanel_UpdateSelectedEquipment(p, 8, 4, 0, 0);
    PlayerPanel_SnapshotValues(p);
    sub_08166534(p, 10, 6);
    InventoryPanel_ShowStatusIcons(p);
    ((struct HudObj *)(p + 0x1720))->flags &= ~1;
    Anim_LoadKeyframe(p + 0x1720, p + 0x64, 0x66, 0);
    ((struct HudObj *)(p + 0x1780))->flags &= ~1;
    Anim_LoadKeyframe(p + 0x1780, p + 0x64, 0x70, 0);
    PauseInventory_InitConsumableCursors(p);
    SceneHud_LayoutAlternateActionGrid(p);
    sub_08170D78(p);
    PauseMenu_SetPromptIconPositions(p, 0, 0);
    MenuDisplay_UpdateActionPrompt(p, 3);
    Hud_SetInteractionCallback(p, PauseInventory_UpdateConsumableActions, 0);
    MenuDisplay_SetPaletteOverrides(p, 0, 1, 0, 0);
    ScrollingDialogue_HideImmediately();
}
