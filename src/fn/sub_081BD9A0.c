#include "global.h"
#include "input.h"

#define MenuPanel_CyclePage Storage_TickCategorySelection
struct MenuPanel {
    u8 pad0[0x1754];
    u8 page;
    u8 pageCount;
};

s32 Storage_TickCategorySelection(struct MenuPanel *panel)
{
    u32 left = (u16)(gPadState[1] & 0x200);
    if (left) {
        u32 page = panel->page;
        if (page == 0) {
            page = panel->pageCount;
        }
        panel->page = page - 1;
        return 1;
    }
    if (((s32)gPadState[1]) & 0x100) {
        panel->page++;
        if (panel->page >= panel->pageCount) {
            panel->page = left;
        }
        return 1;
    }
    return 0;
}
