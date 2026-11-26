#ifndef GUARD_SHOP_TAIL_H
#define GUARD_SHOP_TAIL_H
#include "ui/widgets/scene_input_navigation.h"
#include "ui/widgets/scene_input_panel.h"
#include "ui/town/shop_callback.h"
/* Phase-specific controller view; the catalog/transition workspace overlaps it. */
struct ShopTail {
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
s32 Shop_HandleClockShortcut(struct ShopTail *);
s32 Shop_HandlePageKeys(struct ShopTail *);
void Shop_UpdateModePrompt(struct ShopTail *, s32);
void Shop_BeginBrowse(struct ShopTail *);
void Shop_ConfirmEquipmentAction(struct ShopTail *);
void Shop_ConfirmItemAction(struct ShopTail *);
void Shop_ConfirmConsumableAction(struct ShopTail *);
void Shop_BeginAlternateItemSelection(struct ShopTail *);
void Shop_BeginAlternateSelection(struct ShopTail *);
s32 Shop_ClassifyEquipmentMove(struct ShopTail *);
s32 Shop_ClassifyItemMove(struct ShopTail *);
s32 Shop_ClassifyConsumableMove(struct ShopTail *);
void Shop_BeginSwapTransition(struct ShopTail *);
void Shop_BeginPurchaseConfirmation(struct ShopTail *);
void Shop_BeginSaleConfirmation(struct ShopTail *);
void Shop_BeginThirdSelection(struct ShopTail *);
#endif
