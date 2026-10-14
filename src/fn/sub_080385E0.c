#include "global.h"
// Tentative menu/save views; field offsets are observed in this controller.
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
extern u32 gUnk_03003A50;
extern u16 gUnk_03005260[];
void sub_080385E0(struct TradeMenu *p)
{
    struct TradeSave *save = gUnk_02000710;
    s16 *selected = &save->leftSelected;
    int row;
    int item;
    if (p) {
        row = *selected;
    } else {
        row = *selected;
    }
    if (row > 7) {
        item = *((s16 *)((((u8 *)save) + (row * 2)) + 0x68));
        if (item >= 0) {
            p->extraMask &= ~(1 << item);
        }
    } else {
        item = *((s16 *)((((u8 *)save) + ((*selected) * 2)) + 0x68));
        p->leftMask &= ~(1 << item);
    }
}
