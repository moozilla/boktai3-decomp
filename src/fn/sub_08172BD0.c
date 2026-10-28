#include "global.h"
#include "scene_hud_display.h"
void MenuDisplay_UpdateActionPrompt(void *, int);
void InventoryPanel_SetEntryRow(void *, u32, u32, u32, u32, u32);
u32 InventoryPanel_UpdateSelectedUnlock(void *, int, int, int, int);
void InventoryPanel_DrawStatComparison(void *, int, int, int, int, int);
int SceneInput_NavigateGridKeys(void *, int);
void Hud_SaveGridSelection(void *);
void SceneHud_RefreshEquippedActionIcons(void *);
void PauseMenu_RefreshFruitAndUnlockIcons(void *);
void SceneHud_SetActionCursorPosition(void *, u32);
void PauseInventory_ReturnToEquipmentSelection(void *);
void SolarFruit_ToggleEquipSlot(void *);
u8 GameState_ToggleUnlockEquipSlot(void *);
void SceneHud_ShowSelectionDescription(void *);
void SceneHud_UpdateActionGridInput(struct SceneHudState *p)
{
    u32 initialGroup = p->actionGridGroup;
    s32 unlockGroup = 1;
    s32 moved;
    void *grid;
    u8 *cursor;
    if (initialGroup <= 1)
        unlockGroup = 0;
    grid = p->actionInput;
    moved = SceneInput_NavigateGridKeys(grid, 0xF0);
    if (moved) {
        SceneHud_SetActionCursorPosition(&p->categoryWidgets[5].position,
                                         *(cursor = &p->actionCursor));
        SceneHud_ShowSelectionDescription(p);
        Hud_SaveGridSelection(grid);
        {
            u32 selected = *cursor;
            s32 group = 1;
            if (selected <= 7)
                group = 0;
            MenuDisplay_UpdateActionPrompt(p, group);
        }
        if (*cursor > 7)
            InventoryPanel_DrawStatComparison(p, 1, 0, 0, 0, 0);
        else
            InventoryPanel_UpdateSelectedUnlock(p, 3, 14, 0, 0);
    } else if (gPadState[1] & 1) {
        if (!unlockGroup)
            SolarFruit_ToggleEquipSlot(grid);
        else if (GameState_ToggleUnlockEquipSlot(grid)) {
            u32 result = InventoryPanel_UpdateSelectedUnlock(p, 0, 0, 1, moved);
            InventoryPanel_SetEntryRow(p, 3, 14, result, moved, 0x47);
        }
        PauseMenu_RefreshFruitAndUnlockIcons(p);
        {
            u32 selected = p->actionCursor;
            s32 group = 1;
            if (selected <= 7)
                group = 0;
            MenuDisplay_UpdateActionPrompt(p, group);
        }
        SceneHud_RefreshEquippedActionIcons(p);
    } else if (gPadState[1] & 2) {
        Sound_PlaySong(0xDE);
        PauseInventory_ReturnToEquipmentSelection(p);
    }
}
