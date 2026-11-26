#include "ui/town/shop_control.h"
static inline s32 Shop_TurnPage(struct SceneInventoryControlView *panel)
{
    Sound_PlaySong(0x10A);
    panel->interactionFlags |= 0x40;
    Shop_UpdateMixedCatalogCategory(panel);
    return 1;
}
s32 Shop_HandlePageKeys(struct SceneInventoryControlView *panel)
{
    if (panel->interactionFlags & 1) {
        if (gPadState[1] & 0x200) {
            u32 page = panel->page;
            if (page == 0)
                page = panel->pageCount;
            panel->page = page - 1;
            return Shop_TurnPage(panel);
        }
        if (gPadState[1] & 0x100) {
            s32 current = panel->page;
            if (current < panel->pageCount - 1)
                panel->page = current + 1;
            else
                panel->page = 0;
            return Shop_TurnPage(panel);
        }
    }
    return 0;
}
