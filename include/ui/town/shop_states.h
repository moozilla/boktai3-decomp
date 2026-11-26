#ifndef GUARD_SCENE_INVENTORY_STATES_H
#define GUARD_SCENE_INVENTORY_STATES_H
#include "ui/town/shop_control.h"
struct SceneInventoryActionView;
struct SceneInventoryConsumableSlot {
    u8 unknown0[0x160];
    s16 id;
};
void SceneInput_SaveSelection(struct SceneInputGridView *);
void Shop_BeginActionSelection(struct SceneInventoryControlView *, s32);
void SceneInputPanel_ShowDescription(void *, s32, const void *, s32);
s32 Shop_TickModeMenu(struct SceneInventoryControlView *);
s32 Shop_TickMoveSelection(struct SceneInventoryControlView *);
s32 Shop_TickResetConfirmation(struct SceneInventoryControlView *);
s32 Shop_TickPurchaseConfirmation(struct SceneInventoryControlView *);
s32 Shop_TickPurchaseTransition(struct SceneInventoryControlView *);
s32 sub_080624C8(struct SceneInventoryControlView *);
s32 Shop_TickSaleConfirmation(struct SceneInventoryControlView *);
s32 Shop_TickSaleTransition(struct SceneInventoryControlView *);
s32 Shop_TickSaleFeedback(struct SceneInventoryControlView *);
s32 Shop_TickDiscardConfirmation(struct SceneInventoryControlView *);
s32 Shop_TickDiscardTransition(struct SceneInventoryControlView *);
s32 Shop_TickSwapTransition(struct SceneInventoryControlView *);
void Shop_BeginEquipmentSwapPair(struct SceneInventoryActionView *);
void Shop_BeginEquipmentSpecialMove(struct SceneInventoryActionView *);
void Shop_BeginEquipmentRemoveMove(struct SceneInventoryActionView *);
void Shop_BeginEquipmentCatalogMove(struct SceneInventoryActionView *);
void Shop_BeginConsumableRemoveMove(struct SceneInventoryActionView *);
void Shop_BeginConsumableSwapPair(struct SceneInventoryActionView *);
void Shop_BeginConsumableSpecialMove(struct SceneInventoryActionView *);
void Shop_BeginConsumableCatalogMove(struct SceneInventoryActionView *);
void Shop_BeginItemRemoveMove(struct SceneInventoryActionView *);
void Shop_BeginItemSwapPair(struct SceneInventoryActionView *);
void Shop_BeginItemSpecialMove(struct SceneInventoryActionView *);
void Shop_BeginItemCatalogMove(struct SceneInventoryActionView *);
#endif
