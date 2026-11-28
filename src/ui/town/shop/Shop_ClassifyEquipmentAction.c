#include "ui/town/shop_control.h"
s32 Shop_ClassifyEquipmentAction(struct ShopControlView *panel)
{
    s32 result;
    if (panel->grid.selection > 15) {
        s32 entry;
        result = 1;
        {
            u8 raw = ((struct ShopCatalogView *)panel)
                         ->entries[panel->grid.selection - 16 + (panel->page << 3)];
            entry = (s8)raw;
        }
        if (entry == 255 || entry < 0 ||
            gGameState->unk7B0 < Equipment_GetDefinition(entry)->baseValue ||
            panel->destination < 0)
            result = 0;
    } else {
        s32 selected = panel->grid.selection;
        s32 assignable;
        Inventory_GetEquipmentId(selected);
        assignable = Inventory_CanAssignEquipment(selected);
        result = -1;
        if (!assignable)
            result = 0;
    }
    return result;
}
