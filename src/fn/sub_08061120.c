#include "scene_inventory_control.h"
s32 SceneInventory_ClassifyEquipmentAction(struct SceneInventoryControlView *panel)
{
    s32 result;
    if (panel->grid.selection > 15) {
        s32 entry;
        result = 1;
        {
            s32 pageIndex = (panel->page << 3) - 16;
            u8 raw;
            raw = ((struct SceneInventoryCatalogView *)panel)
                      ->entries[pageIndex + panel->grid.selection];
            entry = (s8)raw;
        }
        if (entry == 255 || entry < 0)
            goto invalid;
        {
            s32 funds = gGameState->unk7B0;
            if (funds < Equipment_GetDefinition(entry)->baseValue)
                goto invalid;
        }
        if (panel->destination >= 0)
            goto done;
        goto invalid;
    } else {
        s32 selected = panel->grid.selection;
        Inventory_GetEquipmentId(selected);
        {
            s32 equipped = Inventory_CanAssignEquipment(selected);
            result = -1;
            if (!equipped)
                goto invalid;
        }
    }
    goto done;
invalid:
    result = 0;
done:
    return result;
}
