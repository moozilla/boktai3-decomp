#include "global.h"

#ifdef NONMATCHING
// fuzzy: 96.4%
// foundry draft (worker best 96.4%); what differs is mostly register allocation and instruction order
#define MenuPanel_CyclePage Storage_TickCategorySelection
#define gMenuKeys           gUnk_03005260
struct MenuPanel {
    u8 pad0[0x1754];
    u8 page;
    u8 pageCount;
};
extern u16 gUnk_03005260[];

s32 Storage_TickCategorySelection(struct MenuPanel *panel)
{
    u16 keys = gUnk_03005260[1];
    u32 left = (u16)(keys & 0x200);
    if (left) {
        u32 page = panel->page;
        if (page == 0)
            page = panel->pageCount;
        panel->page = page - 1;
        return 1;
    }
    if ((s32)keys & 0x100) {
        panel->page++;
        if (panel->page >= panel->pageCount)
            panel->page = left;
        return 1;
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", Storage_TickCategorySelection);
#endif
