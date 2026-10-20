#ifndef GUARD_SCENE_INVENTORY_STATES_H
#define GUARD_SCENE_INVENTORY_STATES_H
#include "scene_inventory_control.h"
struct SceneInventoryActionView;
struct SceneInventoryConsumableSlot {
    u8 unknown0[0x160];
    s16 id;
};
void SceneInput_SaveSelection(struct SceneInputGridView *);
void SceneInventory_BeginActionSelection(struct SceneInventoryControlView *, s32);
void SceneInputPanel_ShowDescription(void *, s32, const void *, s32);
s32 SceneInventory_TickModeMenu(struct SceneInventoryControlView *);
s32 SceneInventory_TickMoveSelection(struct SceneInventoryControlView *);
s32 SceneInventory_TickResetConfirmation(struct SceneInventoryControlView *);
s32 SceneInventory_TickPurchaseConfirmation(struct SceneInventoryControlView *);
s32 SceneInventory_TickPurchaseTransition(struct SceneInventoryControlView *);
s32 sub_080624C8(struct SceneInventoryControlView *);
s32 SceneInventory_TickSaleConfirmation(struct SceneInventoryControlView *);
s32 SceneInventory_TickSaleTransition(struct SceneInventoryControlView *);
s32 SceneInventory_TickSaleFeedback(struct SceneInventoryControlView *);
s32 SceneInventory_TickDiscardConfirmation(struct SceneInventoryControlView *);
s32 SceneInventory_TickDiscardTransition(struct SceneInventoryControlView *);
s32 SceneInventory_TickSwapTransition(struct SceneInventoryControlView *);
void SceneInventory_BeginEquipmentSwapPair(struct SceneInventoryActionView *);
void SceneInventory_BeginEquipmentSpecialMove(struct SceneInventoryActionView *);
void SceneInventory_BeginEquipmentRemoveMove(struct SceneInventoryActionView *);
void SceneInventory_BeginEquipmentCatalogMove(struct SceneInventoryActionView *);
void SceneInventory_BeginConsumableRemoveMove(struct SceneInventoryActionView *);
void SceneInventory_BeginConsumableSwapPair(struct SceneInventoryActionView *);
void SceneInventory_BeginConsumableSpecialMove(struct SceneInventoryActionView *);
void SceneInventory_BeginConsumableCatalogMove(struct SceneInventoryActionView *);
void SceneInventory_BeginItemRemoveMove(struct SceneInventoryActionView *);
void SceneInventory_BeginItemSwapPair(struct SceneInventoryActionView *);
void SceneInventory_BeginItemSpecialMove(struct SceneInventoryActionView *);
void SceneInventory_BeginItemCatalogMove(struct SceneInventoryActionView *);
#endif
