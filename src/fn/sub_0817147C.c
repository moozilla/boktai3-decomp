#include "global.h"
void sub_08170D78(void *);
void Hud_RestoreSecondaryGridSelection(void *);
void SceneInput_BuildActionGrid(void *, int);
void PauseMenu_ClearSpriteFlags(void *, int, int);
void PauseMenu_AddSpriteFlags(void *, int, int);
void PlayerPanel_SnapshotValues(void *);
void sub_08166534(void *, int, int);
void InventoryPanel_UpdateSelectedUnlock(void *, int, int, int, int);
void InventoryPanel_UpdateSelectedEquipment(void *, int, int, int, int);
void InventoryPanel_ShowStatusIcons(void *);
void PauseInventory_UpdateConsumableActions(void);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void MessageWindow_Clear(void);
void MenuDisplay_SetPaletteOverrides(void *, int, int, int, int);
void sub_08170F54(void *);
void MenuDisplay_UpdateActionPrompt(void *, int);
void PauseInventory_ReturnToConsumableActions(u8 *p)
{
    u8 *q;
    int z;
    sub_08170D78(p);
    q = p + 0x4384;
    Hud_RestoreSecondaryGridSelection(q);
    SceneInput_BuildActionGrid(q, 0xe1);
    PauseMenu_ClearSpriteFlags(p, 4, 1);
    PauseMenu_AddSpriteFlags(p, 6, 1);
    PauseMenu_AddSpriteFlags(p, 5, 1);
    PlayerPanel_SnapshotValues(p);
    sub_08166534(p, 10, 6);
    z = 0;
    InventoryPanel_UpdateSelectedUnlock(p, 8, 3, 0, z);
    InventoryPanel_UpdateSelectedEquipment(p, 8, 4, 0, z);
    InventoryPanel_ShowStatusIcons(p);
    Hud_SetInteractionCallback(p, PauseInventory_UpdateConsumableActions, 0);
    MessageWindow_Clear();
    MenuDisplay_SetPaletteOverrides(p, 0, 1, 0, z);
    sub_08170F54(p);
    MenuDisplay_UpdateActionPrompt(p, 3);
}
