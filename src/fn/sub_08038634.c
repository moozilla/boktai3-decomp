#include "global.h"
// Tentative menu/save views: offsets observed in this controller.
struct TradeMenu {
    u8 pad0[0x24];
    u32 timer;
    u8 state, entered;
    u8 pad2a[0x36];
    void *window;
    u32 count, leftCount, rightCount, leftMask, rightMask, extraMask;
};
struct TradeSave {
    u8 pad0[0x50];
    s16 leftSelected, rightSelected;
    u8 pad54[0x14];
    s16 leftRows[16], rightRows[16];
};
extern struct TradeSave *gUnk_02000710;
void sub_08038634(struct TradeMenu *p)
{
    struct TradeSave *save = gUnk_02000710;
    int row = save->rightSelected;
    int item = *(s16 *)((u8 *)save + row * 2 + 0x88);
    if (item >= 0) {
        int flags = p->rightMask & ~(1 << item);
        p->rightMask = flags;
        if ((u32)(item - 1) <= 1) p->rightMask = flags & ~6;
    }
}
void sub_08038674(struct TradeMenu *p)
{
    if (p->entered) p->entered = 0;
}
