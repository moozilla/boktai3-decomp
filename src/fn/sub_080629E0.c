#include "scene_inventory_tail.h"
#include "scene_inventory_actions.h"
#include "scene_inventory_panel.h"
#include "scene_inventory_catalog.h"
#include "scene_hud_display.h"
void SceneInput_PositionCursor(struct SceneInputGridView *, struct SceneInputWidget *);
void SceneHud_HideFourDigits(struct SceneHudDigitView *);
void SceneInputPanel_TickHighlight(struct SceneInputHighlight *);
void SceneInputPanel_SetMeterGlyph(struct SceneInputPanel *, u32, u32);
void SceneInventory_PopulateEquipmentGrid(struct SceneInventoryPanel *);
void SceneInventory_PopulateItemGrid(struct SceneInventoryPanel *);
void SceneInventory_PopulateConsumableGrid(struct SceneInventoryPanel *);
void SceneInventory_RefreshEquipmentValue(struct SceneInventoryActionView *);
void SceneInventory_RefreshItemValue(struct SceneInventoryActionView *);
void SceneInventory_RefreshConsumableValue(struct SceneInventoryActionView *);
void sub_0805F904(struct SceneInventoryTail *);
void SceneInventory_ShowItemDescription(struct SceneInventoryPanel *);
void SceneInventory_ShowConsumableDescription(struct SceneInventoryPanel *);
void SceneInventory_ClearDescription(struct SceneInventoryTail *);
void SceneInventory_UpdateDisplay(struct SceneInventoryTail *p)
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
        SceneHud_HideFourDigits((struct SceneHudDigitView *)p);
    if (p->interactionFlags & 0x40) {
        switch (p->category) {
            case 0:
                SceneInventory_PopulateEquipmentGrid((struct SceneInventoryPanel *)p);
                break;
            case 1:
                SceneInventory_PopulateItemGrid((struct SceneInventoryPanel *)p);
                break;
            case 2:
                SceneInventory_PopulateConsumableGrid((struct SceneInventoryPanel *)p);
                break;
        }
    }
    if (p->interactionFlags & 0xC0) {
        switch (p->category) {
            case 0:
                if (p->interactionFlags & 0x10)
                    SceneInventory_RefreshEquipmentValue((struct SceneInventoryActionView *)p);
                if (p->interactionFlags & 0x20)
                    sub_0805F904(p);
                else
                    SceneInventory_ClearDescription(p);
                break;
            case 1:
                if (p->interactionFlags & 0x10)
                    SceneInventory_RefreshItemValue((struct SceneInventoryActionView *)p);
                if (p->interactionFlags & 0x20)
                    SceneInventory_ShowItemDescription((struct SceneInventoryPanel *)p);
                else
                    SceneInventory_ClearDescription(p);
                break;
            case 2:
                if (p->interactionFlags & 0x10)
                    SceneInventory_RefreshConsumableValue((struct SceneInventoryActionView *)p);
                if (p->interactionFlags & 0x20)
                    SceneInventory_ShowConsumableDescription((struct SceneInventoryPanel *)p);
                else
                    SceneInventory_ClearDescription(p);
                break;
        }
    }
    p->interactionFlags &= 0x3F;
    SceneInputPanel_SetMeterGlyph(&p->display, p->page,
                                  ((struct SceneInventoryScriptCatalog *)p)->pageCount);
    SceneInputPanel_TickHighlight(&p->highlight);
}
