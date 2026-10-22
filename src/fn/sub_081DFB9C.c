#include "global.h"
extern u8 gUnk_0824FD44[];
void memcpy(void *, const void *, u32);
struct DFB9CPanel {
    u8 unknown0[0x130];
    u32 flags;
    u8 unknown134[0x14];
    s16 x, y;
    u8 unknown14C[0x204];
    s32 index;
};
void sub_081DFB9C(struct DFB9CPanel *p)
{
    u8 tbl[12];
    s32 i;
    memcpy(tbl, gUnk_0824FD44, 12);
    i = p->index;
    if (i <= 11) {
        p->x = tbl[i];
        p->y = 0;
    } else {
        p->x = tbl[i - 12];
        p->y = 0x10;
    }
    p->x -= 0x80;
    p->y += 0x10;
    p->flags &= ~1;
}
