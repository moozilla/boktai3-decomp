// MATCHING_CONCESSION: register-pinning
#include "scene_inventory_control.h"
#define SceneInventory_ClassifyEquipmentAction sub_08061120
s32 SceneInventory_ClassifyEquipmentAction(struct SceneInventoryControlView *panel)
{
    s32 result;
    if (panel->grid.selection > 15) {
        s32 entry;
        result = 1;
        {
            s32 pageIndex = (panel->page << 3) - 16;
            register u8 raw asm("r1");
            raw = ((struct SceneInventoryCatalogView *)panel)
                      ->entries[pageIndex + panel->grid.selection];
            entry = (s8)raw;
        }
        if (entry == 255 || entry < 0)
            goto invalid;
        {
            s32 funds = gUnk_02000710->unk7B0;
            if (funds < sub_08227F5C(entry)->baseValue)
                goto invalid;
        }
        if (panel->destination >= 0)
            goto done;
        goto invalid;
    } else {
        s32 selected = panel->grid.selection;
        sub_0822D6E0(selected);
        {
            s32 equipped = sub_0822DFDC(selected);
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
