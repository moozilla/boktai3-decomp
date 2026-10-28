// MATCHING_CONCESSION: register-pinning
#include "scene_inventory_control.h"
s32 SceneInventory_HandlePageKeys(struct SceneInventoryControlView *panel)
{
    if (panel->interactionFlags & 1) {
        register u32 keys asm("r1") = gPadState[1];
        u32 bits = keys & 0x200;
        u32 left;
        left = (u16)bits;
        if (left) {
            u32 page = panel->page;
            if (page == 0)
                page = panel->pageCount;
            panel->page = page - 1;
        } else if (keys & 0x100) {
            u8 *page;
            s32 current;
            {
                register u32 offset asm("r2") = 0x1AB0;
                asm("" : "+r"(offset));
                page = (void *)((u32)panel + offset);
            }
            current = *page;
            if (current >= panel->pageCount - 1)
                *page = left;
            else
                *page = current + 1;
        } else
            return 0;
        Sound_PlaySong(0x10A);
        panel->interactionFlags |= 0x40;
        SceneInventory_UpdateMixedCatalogCategory(panel);
        return 1;
    }
    return 0;
}
