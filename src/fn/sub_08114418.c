#include "global.h"

extern u8 *gUnk_020004C8;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_081143D8(u8 *);
void sub_0821A0C0(u8 *);
void sub_08114370(void);
void sub_081143B8(void);

u8 *sub_08114418(void)
{
    u8 *p;
    if (gUnk_020004C8 != 0)
        return gUnk_020004C8;
    p = sub_08219FBC(0xa, 0xd9c);
    if (p != 0) {
        sub_0821A04C(p, sub_08114370, sub_081143B8);
        if (sub_081143D8(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
