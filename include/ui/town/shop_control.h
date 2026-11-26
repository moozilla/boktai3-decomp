#ifndef GUARD_SHOP_CONTROL_H
#define GUARD_SHOP_CONTROL_H
#include "ui/widgets/scene_input_panel.h"
#include "ui/widgets/scene_input_navigation.h"
#include "system/game_state/game_state.h"
#include "ui/town/shop_callback.h"
/* Catalog and transition/controller storage overlap across UI phases. */
struct ShopControlView {
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
struct ShopCatalogView {
    u8 unknown0[0x1AB3];
    s8 entries[0x134];
};
struct ShopHighlightView {
    u8 unknown0[0x1D00];
    u16 colors[16];
    u8 phase;
};
struct ShopCallbackView {
    u8 unknown0[0x1AEE];
    u16 frame;
    u8 unknown1AF0[0x1CFC - 0x1AF0];
    ShopCallback callback;
};
struct ShopControlDefinition {
    u8 unknown0[6];
    u16 baseValue;
};
struct ShopControlConsumable {
    u8 unknown0[8];
    s16 baseValue;
};
s32 Inventory_GetItemId(u32);
s32 Inventory_GetEquipmentId(u32);
s32 Inventory_CanAssignEquipment(u32);
s32 GameState_IsConsumableSlotEquipped(u32);
struct ShopControlDefinition *Equipment_GetDefinition(u32);
struct ShopControlConsumable *GameState_GetConsumableDescriptor(u32);
u32 SceneItemValue_GetItemBaseValue(s32);
s32 SceneItemValue_GetConsumableBaseValue(u32);
s32 Shop_ClassifyEquipmentAction(struct ShopControlView *);
s32 Shop_ClassifyItemAction(struct ShopControlView *);
s32 Shop_ClassifyConsumableAction(struct ShopControlView *);
s32 Shop_ClassifySelectedAction(struct ShopControlView *);
void Shop_UpdateActionPrompt(struct ShopControlView *);
void Shop_UpdateModePrompt(struct ShopControlView *, s32);
s32 Shop_FindActionDestination(struct ShopControlView *);
void Shop_SelectActionDestination(struct ShopControlView *);
void SceneInputPanel_PositionModeCursor(struct SceneInputPanel *, u32);
void SceneInput_RestoreSelection(struct SceneInputGridView *);
void SceneInput_InitGrid(struct SceneInputGridView *, u32, u32, u32, u32);
s32 Shop_FindEmptyEquipmentSlot(const void *);
s32 Shop_FindEmptyItemSlot(const void *);
s32 Shop_FindEmptyConsumableSlot(const void *);
s32 Shop_TickActionSelection(struct ShopControlView *);
s32 Shop_HandleClockShortcut(struct ShopControlView *);
u32 Shop_UpdateMixedCatalogCategory(struct ShopControlView *);
s32 Shop_HandlePageKeys(struct ShopControlView *);
void Shop_BeginMoveSelection(struct ShopControlView *);
void Shop_BeginItemSelection(struct ShopControlView *);
void Shop_BeginExitConfirmation(struct ShopControlView *);
void Shop_BeginConsumableSelection(struct ShopControlView *);
void Shop_BeginPurchaseConfirmation(struct ShopControlView *);
void Shop_BeginSaleConfirmation(struct ShopControlView *);
void Shop_BeginReposition(struct ShopControlView *);
void Shop_BeginRemovalConfirmation(struct ShopControlView *);
void SceneInputPanel_HighlightMode(struct SceneInputPanel *, u16 *);
extern const u8 gUnk_0824DF38[];
#endif
