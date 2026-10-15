#ifndef GUARD_SCENE_INVENTORY_CONTROL_H
#define GUARD_SCENE_INVENTORY_CONTROL_H
#include "scene_input_panel.h"
#include "scene_input_navigation.h"
#include "game_state.h"
#include "scene_inventory_callback.h"
/* Catalog and transition/controller storage overlap across UI phases. */
struct SceneInventoryControlView {
    struct SceneInputPanel display;
    u8 unknown14E8[0x1A8C - 0x14E8];
    const void *equipmentText;
    u32 unknown1A90;
    const void *itemText, *consumableText;
    u8 unknown1A9C[8];
    u8 interactionFlags, modeIndex, mode, catalogMode, category;
    u8 unknown1AA9, choiceValue, clockMode, unknown1AAC[3], choiceBlink;
    u8 page, pageCount, itemPageCount;
    u8 unknown1AB3[0x1AEC - 0x1AB3];
    s16 destination;
    u16 frame;
    u8 unknown1AF0[0x1BE4 - 0x1AF0];
    struct SceneInputGridView grid;
    struct SceneInputPair prompt;
};
struct SceneInventoryCatalogView {
    u8 unknown0[0x1AB3];
    s8 entries[0x134];
};
struct SceneInventoryHighlightView {
    u8 unknown0[0x1D00];
    u16 colors[16];
    u8 phase;
};
struct SceneInventoryCallbackView {
    u8 unknown0[0x1AEE];
    u16 frame;
    u8 unknown1AF0[0x1CFC - 0x1AF0];
    SceneInventoryCallback callback;
};
struct SceneInventoryControlDefinition {
    u8 unknown0[6];
    u16 baseValue;
};
struct SceneInventoryControlConsumable {
    u8 unknown0[8];
    s16 baseValue;
};
s32 sub_0822CDA8(u32);
s32 sub_0822D6E0(u32);
s32 sub_0822DFDC(u32);
s32 sub_0822E224(u32);
struct SceneInventoryControlDefinition *sub_08227F5C(u32);
struct SceneInventoryControlConsumable *sub_0822E288(u32);
u32 SceneItemValue_GetItemBaseValue(s32);
s32 SceneItemValue_GetConsumableBaseValue(u32);
s32 SceneInventory_ClassifyEquipmentAction(struct SceneInventoryControlView *);
s32 SceneInventory_ClassifyItemAction(struct SceneInventoryControlView *);
s32 SceneInventory_ClassifyConsumableAction(struct SceneInventoryControlView *);
s32 SceneInventory_ClassifySelectedAction(struct SceneInventoryControlView *);
void SceneInventory_UpdateActionPrompt(struct SceneInventoryControlView *);
void SceneInventory_UpdateModePrompt(struct SceneInventoryControlView *, s32);
s32 SceneInventory_FindActionDestination(struct SceneInventoryControlView *);
void SceneInventory_SelectActionDestination(struct SceneInventoryControlView *);
void SceneInputPanel_PositionModeCursor(struct SceneInputPanel *, u32);
void SceneInput_RestoreSelection(struct SceneInputGridView *);
void SceneInput_InitGrid(struct SceneInputGridView *, u32, u32, u32, u32);
s32 SceneInventory_FindEmptyEquipmentSlot(const void *);
s32 SceneInventory_FindEmptyItemSlot(const void *);
s32 SceneInventory_FindEmptyConsumableSlot(const void *);
s32 SceneInventory_TickActionSelection(struct SceneInventoryControlView *);
s32 SceneInventory_HandleClockShortcut(struct SceneInventoryControlView *);
u32 SceneInventory_UpdateMixedCatalogCategory(struct SceneInventoryControlView *);
s32 SceneInventory_HandlePageKeys(struct SceneInventoryControlView *);
void SceneInventory_BeginMoveSelection(struct SceneInventoryControlView *);
void SceneInventory_BeginItemSelection(struct SceneInventoryControlView *);
void SceneInventory_BeginExitConfirmation(struct SceneInventoryControlView *);
void SceneInventory_BeginConsumableSelection(struct SceneInventoryControlView *);
void SceneInventory_BeginPurchaseConfirmation(struct SceneInventoryControlView *);
void SceneInventory_BeginSaleConfirmation(struct SceneInventoryControlView *);
void SceneInventory_BeginReposition(struct SceneInventoryControlView *);
void SceneInventory_BeginRemovalConfirmation(struct SceneInventoryControlView *);
void SceneInputPanel_HighlightMode(struct SceneInputPanel *, u16 *);
extern const u8 gUnk_0824DF38[];
#endif
