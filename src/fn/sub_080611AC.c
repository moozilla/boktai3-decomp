// MATCHING_CONCESSION: register-pinning
#include "scene_inventory_control.h"
#define SceneInventory_ClassifyItemAction sub_080611AC
s32 SceneInventory_ClassifyItemAction(struct SceneInventoryControlView *panel)
{
    s32 result;
    if (panel->grid.selection > 15) {
        s32 pageIndex = (panel->page << 3) - 16;
        u8 raw =
            ((struct SceneInventoryCatalogView *)panel)->entries[pageIndex + panel->grid.selection];
        s32 entry = (s8)raw;
        result = 1;
        if (entry < 0)
            goto invalid;
        {
            s32 funds = gUnk_02000710->unk7B0;
            if (funds < (s32)sub_0805F86C(entry))
                goto invalid;
        }
        if (panel->destination >= 0)
            goto done;
        goto invalid;
    } else {
        register s32 item asm("r1") = sub_0822CDA8(panel->grid.selection);
        asm("" : "+r"(item));
        result = -1;
        if (item < 0)
            goto invalid;
    }
    goto done;
invalid:
    result = 0;
done:
    return result;
}
