#include "global.h"
void PauseMenu_CloseTextWindows(void *);
void Hud_SetupBackgroundLayer(void *, u32, int, int, u32);
void PauseMenu_HideAllSprites(void *);
void PauseMenu_LoadSpriteKeyframe(void *, int, int);
void PauseMenu_ClearSpriteFlags(void *, int, int);
void sub_0816618C(void *, int, int);
void sub_08172A70(void *);
void Hud_DrawHeaderCounter(void);
void SceneHud_LayoutActionGrid_08172950(void *);
void SceneHud_RefreshEquippedActionIcons(void *);
void PauseMenu_SetPromptIconPositions(void *, int, int);
void MenuDisplay_UpdateActionPrompt(void *, int);
void sub_081731E8(void);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void MenuDisplay_SetPaletteOverrides(void *, int, int, int, int);
void ScrollingDialogue_HideImmediately(void);
void PlayerPanel_SnapshotValues(void *);
void InventoryPanel_UpdateSelectedUnlock(void *, int, int, int, int);
void PauseInventory_InitEquipmentSelectionPage(u8 *p)
{
    int z;
    PauseMenu_CloseTextWindows(p);
    Hud_SetupBackgroundLayer(p, *(u32 *)(p + 0x18), 0, 7, *(u32 *)(p + 0x2c));
    PauseMenu_HideAllSprites(p);
    PauseMenu_LoadSpriteKeyframe(p, 0, 0x54);
    PauseMenu_ClearSpriteFlags(p, 0x4d, 1);
    sub_0816618C(p, 8, 0x20);
    PauseMenu_LoadSpriteKeyframe(p, 8, 0x71);
    sub_08172A70(p);
    Hud_DrawHeaderCounter();
    SceneHud_LayoutActionGrid_08172950(p);
    SceneHud_RefreshEquippedActionIcons(p);
    PauseMenu_SetPromptIconPositions(p, 0, 0);
    {
        u32 v = p[0x4387];
        int b = 1;
        if (v <= 7)
            b = 0;
        MenuDisplay_UpdateActionPrompt(p, b);
    }
    Hud_SetInteractionCallback(p, sub_081731E8, 0);
    z = 0;
    MenuDisplay_SetPaletteOverrides(p, 0, 1, 0, z);
    ScrollingDialogue_HideImmediately();
    PlayerPanel_SnapshotValues(p);
    InventoryPanel_UpdateSelectedUnlock(p, 3, 14, 0, z);
}
