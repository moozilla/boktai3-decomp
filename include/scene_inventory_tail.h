#ifndef GUARD_SCENE_INVENTORY_TAIL_H
#define GUARD_SCENE_INVENTORY_TAIL_H
#include "scene_input_navigation.h"
#include "scene_input_panel.h"
#include "scene_inventory_callback.h"
/* Phase-specific controller view; the catalog/transition workspace overlaps it. */
struct SceneInventoryTail {
    struct SceneInputPanel display;
    u8 unknown14E8[0x1AA4 - 0x14E8];
    u8 interactionFlags, unknown1AA5, mode, unknown1AA7, category;
    u8 unknown1AA9, choiceState, clockMode, purchased, sold, blinkFrame, choiceRepeat;
    u8 page, unknown1AB1[0x1BE4 - 0x1AB1];
    struct SceneInputGridView grid;
    u8 unknown1C14[0x1D00 - 0x1C14];
    struct SceneInputHighlight highlight;
};
void SceneInputPanel_HighlightCompare(struct SceneInputPanel *, u16 *);
void SceneInputPanel_HighlightPrompt(struct SceneInputPanel *, u16 *);
u32 SceneInput_NavigateGrid(struct SceneInputGridView *);
s32 SceneInventory_HandleClockShortcut(struct SceneInventoryTail *);
s32 SceneInventory_HandlePageKeys(struct SceneInventoryTail *);
void SceneInventory_UpdateModePrompt(struct SceneInventoryTail *, s32);
void SceneInventory_BeginBrowse(struct SceneInventoryTail *);
void SceneInventory_ConfirmEquipmentAction(struct SceneInventoryTail *);
void SceneInventory_ConfirmItemAction(struct SceneInventoryTail *);
void SceneInventory_ConfirmConsumableAction(struct SceneInventoryTail *);
void SceneInventory_BeginAlternateItemSelection(struct SceneInventoryTail *);
void SceneInventory_BeginAlternateSelection(struct SceneInventoryTail *);
s32 SceneInventory_ClassifyEquipmentMove(struct SceneInventoryTail *);
s32 SceneInventory_ClassifyItemMove(struct SceneInventoryTail *);
s32 SceneInventory_ClassifyConsumableMove(struct SceneInventoryTail *);
void SceneInventory_BeginSwapTransition(struct SceneInventoryTail *);
void SceneInventory_BeginPurchaseConfirmation(struct SceneInventoryTail *);
void SceneInventory_BeginSaleConfirmation(struct SceneInventoryTail *);
void SceneInventory_BeginThirdSelection(struct SceneInventoryTail *);
#endif
