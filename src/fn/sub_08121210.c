#include "global.h"

extern u8 *gUnk_0200020C;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_08121204(u8 *);
void sub_0821A0C0(u8 *);
void sub_08121110(void);
void sub_08121154(void);

u8 *sub_08121210(void)
{
    u8 *p;
    if (gUnk_0200020C != 0)
        return gUnk_0200020C;
    p = sub_08219FBC(0x8, 0x920);
    if (p != 0) {
        sub_0821A04C(p, sub_08121110, sub_08121154);
        if (sub_08121204(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
