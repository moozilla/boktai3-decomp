// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_hud_display.h"

#define SceneInventory_IsSlotEquipped Inventory_IsEquipmentAssigned
int SceneInput_NavigateGridKeys(void *, int);
void SceneHud_GetActionIconPosition(void *, int);
void SceneHud_ShowEquipmentDetails(void *);
void Hud_SaveGridSelection(void *);
void MenuDisplay_UpdateActionPrompt(void *, int);
void InventoryPanel_DrawStatComparison(void *, int, int, int, int, int);
s32 Inventory_GetEquipmentId(u32);
u32 Inventory_IsEquipmentAssigned(s32);
s32 Inventory_RemoveAssignedSlot(u32);
s32 Inventory_AddAssignedSlot(u32);
u32 InventoryPanel_UpdateSelectedEquipment(void *, u32, u32, u32, u32);
void SceneHud_FillDigitCells(u32, u32, u32, u32);
void InventoryPanel_SetEntryRow(void *, u32, u32, u32, u32, u32);
void SceneHud_RefreshEquipmentIcons(void *, int);
void SceneHud_BeginEquipmentSelection(void *);
void SceneHud_TickEquipmentToggle(struct SceneHudState *input)
{
    struct SceneHudState *p = input;
    s32 equipped;
    s32 slot;
    void *q;
    DEBUG_NOP();
    q = p->actionInput;
    if (SceneInput_NavigateGridKeys(q, 0xf0)) {
        SceneHud_GetActionIconPosition(&p->categoryWidgets[5].position, p->actionCursor);
        SceneHud_ShowEquipmentDetails(p);
        Hud_SaveGridSelection(q);
        MenuDisplay_UpdateActionPrompt(p, 2);
        InventoryPanel_DrawStatComparison(p, 0, 1, 0, 0, 0);
    } else if (gPadState[1] & 1) {
        slot = p->actionCursor;
        if (Inventory_GetEquipmentId(slot) == 0xff) {
            Sound_PlaySong(0x192);
        } else {
            do {
                equipped = Inventory_IsEquipmentAssigned(slot);
                if (equipped) {
                    if (Inventory_RemoveAssignedSlot(slot)) {
                        goto success;
                    }
                    Sound_PlaySong(0x192);
                } else if (Inventory_AddAssignedSlot(slot) == 1) {
                    u32 frame = InventoryPanel_UpdateSelectedEquipment(p, 0, 0, 1, equipped);
                    SceneHud_FillDigitCells(3, 8, 3, 0);
                    InventoryPanel_SetEntryRow(p, 8, 4, frame, equipped, 0x47);
                success:
                    Sound_PlaySong(0xdd);
                } else {
                    Sound_PlaySong(0x192);
                }
            } while (0);
            MenuDisplay_UpdateActionPrompt(p, 2);
            SceneHud_RefreshEquipmentIcons(p, 0);
        }
    } else if (gPadState[1] & 2) {
        SceneHud_BeginEquipmentSelection(p);
        Sound_PlaySong(0xde);
    }
}
