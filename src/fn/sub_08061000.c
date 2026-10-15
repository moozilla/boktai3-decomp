// MATCHING_CONCESSION: register-pinning
#include "scene_inventory_control.h"
#define SceneInventory_ClassifyItemMove sub_08061000
s32 SceneInventory_ClassifyItemMove(struct SceneInventoryControlView *panel)
{
    if (panel->grid.savedSelection <= 15) {
        s32 first = sub_0822CDA8(panel->grid.savedSelection);
        s32 second = sub_0822CDA8(panel->grid.selection);
        if (panel->grid.savedSelection == panel->grid.selection)
            return 0;
        if (first < 0 && second < 0)
            return 0;
        if (panel->mode == 1)
            return 1;
    } else {
        if (panel->grid.selection == panel->grid.savedSelection) {
            sub_0806147C(panel);
            return 4;
        }
        if (panel->grid.selection <= 15) {
            register s32 second asm("r1") = sub_0822CDA8(panel->grid.selection);
            asm("" : "+r"(second));
            if (second < 0)
                return 4;
        }
    }
    return 0;
}
