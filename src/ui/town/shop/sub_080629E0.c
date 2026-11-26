#include "ui/town/shop_tail.h"
#include "ui/town/shop_panel.h"
#include "ui/town/shop_catalog.h"
#include "ui/pause/scene_hud_display.h"
void SceneInput_PositionCursor(struct SceneInputGridView *, struct AnimSprite *);
void SceneHud_HideFourDigits(struct SceneInputPanel *);
void SceneInputPanel_TickHighlight(struct SceneInputHighlight *);
void SceneInputPanel_SetMeterGlyph(struct SceneInputPanel *, u32, u32);
void Shop_PopulateEquipmentGrid(struct ShopPanel *);
void Shop_PopulateItemGrid(struct ShopPanel *);
void Shop_PopulateConsumableGrid(struct ShopPanel *);
struct ShopActionView;
void Shop_RefreshEquipmentValue(struct ShopActionView *);
void Shop_RefreshItemValue(struct ShopActionView *);
void Shop_RefreshConsumableValue(struct ShopActionView *);
void sub_0805F904(struct ShopTail *);
void Shop_ShowItemDescription(struct ShopPanel *);
void Shop_ShowConsumableDescription(struct ShopPanel *);
void Shop_ClearDescription(struct ShopTail *);
void Shop_UpdateDisplay(struct ShopTail *p)
{
    u32 bits = p->interactionFlags & 3;
    if (bits == 1) {
        ++p->blinkFrame;
        if ((p->blinkFrame >> 5) & 1) {
            p->display.valueA.flags |= bits;
            p->display.valueB.flags |= bits;
        } else {
            p->display.valueA.flags &= ~1;
            p->display.valueB.flags &= ~1;
        }
    } else {
        p->display.valueA.flags |= 1;
        p->display.valueB.flags |= 1;
    }
    if (p->interactionFlags & 4) {
        SceneInput_PositionCursor(&p->grid, &p->display.compareA);
        p->display.compareA.flags &= ~1;
    } else {
        p->display.compareA.flags |= 1;
    }
    if (p->interactionFlags & 8)
        p->display.category.flags &= ~1;
    else
        p->display.category.flags |= 1;
    if (!(p->interactionFlags & 0x10))
        SceneHud_HideFourDigits((struct SceneInputPanel *)p);
    if (p->interactionFlags & 0x40) {
        switch (p->category) {
            case 0:
                Shop_PopulateEquipmentGrid((struct ShopPanel *)p);
                break;
            case 1:
                Shop_PopulateItemGrid((struct ShopPanel *)p);
                break;
            case 2:
                Shop_PopulateConsumableGrid((struct ShopPanel *)p);
                break;
        }
    }
    if (p->interactionFlags & 0xC0) {
        switch (p->category) {
            case 0:
                if (p->interactionFlags & 0x10)
                    Shop_RefreshEquipmentValue((struct ShopActionView *)p);
                if (p->interactionFlags & 0x20)
                    sub_0805F904(p);
                else
                    Shop_ClearDescription(p);
                break;
            case 1:
                if (p->interactionFlags & 0x10)
                    Shop_RefreshItemValue((struct ShopActionView *)p);
                if (p->interactionFlags & 0x20)
                    Shop_ShowItemDescription((struct ShopPanel *)p);
                else
                    Shop_ClearDescription(p);
                break;
            case 2:
                if (p->interactionFlags & 0x10)
                    Shop_RefreshConsumableValue((struct ShopActionView *)p);
                if (p->interactionFlags & 0x20)
                    Shop_ShowConsumableDescription((struct ShopPanel *)p);
                else
                    Shop_ClearDescription(p);
                break;
        }
    }
    p->interactionFlags &= 0x3F;
    SceneInputPanel_SetMeterGlyph(&p->display, p->page,
                                  ((struct ShopScriptCatalog *)p)->pageCount);
    SceneInputPanel_TickHighlight(&p->highlight);
}
