#ifndef GUARD_SHOP_STATES_H
#define GUARD_SHOP_STATES_H
#include "ui/town/shop_control.h"
struct ShopActionView;
struct ShopConsumableSlot {
    u8 unknown0[0x160];
    s16 id;
};
void SceneInput_SaveSelection(struct SceneInputGridView *);
void Shop_BeginActionSelection(struct ShopControlView *, s32);
void SceneInputPanel_ShowDescription(void *, s32, const void *, s32);
s32 Shop_TickModeMenu(struct ShopControlView *);
s32 Shop_TickMoveSelection(struct ShopControlView *);
s32 Shop_TickResetConfirmation(struct ShopControlView *);
s32 Shop_TickPurchaseConfirmation(struct ShopControlView *);
s32 Shop_TickPurchaseTransition(struct ShopControlView *);
s32 sub_080624C8(struct ShopControlView *);
s32 Shop_TickSaleConfirmation(struct ShopControlView *);
s32 Shop_TickSaleTransition(struct ShopControlView *);
s32 Shop_TickSaleFeedback(struct ShopControlView *);
s32 Shop_TickDiscardConfirmation(struct ShopControlView *);
s32 Shop_TickDiscardTransition(struct ShopControlView *);
s32 Shop_TickSwapTransition(struct ShopControlView *);
void Shop_BeginEquipmentSwapPair(struct ShopActionView *);
void Shop_BeginEquipmentSpecialMove(struct ShopActionView *);
void Shop_BeginEquipmentRemoveMove(struct ShopActionView *);
void Shop_BeginEquipmentCatalogMove(struct ShopActionView *);
void Shop_BeginConsumableRemoveMove(struct ShopActionView *);
void Shop_BeginConsumableSwapPair(struct ShopActionView *);
void Shop_BeginConsumableSpecialMove(struct ShopActionView *);
void Shop_BeginConsumableCatalogMove(struct ShopActionView *);
void Shop_BeginItemRemoveMove(struct ShopActionView *);
void Shop_BeginItemSwapPair(struct ShopActionView *);
void Shop_BeginItemSpecialMove(struct ShopActionView *);
void Shop_BeginItemCatalogMove(struct ShopActionView *);
#endif
