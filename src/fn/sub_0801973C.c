#include "global.h"

extern u8 *gUnk_020000A4;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_08019698(u8 *);
void sub_0821A0C0(u8 *);
void sub_08019628(void);
void sub_08019670(void);

u8 *sub_0801973C(void)
{
    u8 *p;
    if (gUnk_020000A4 != 0)
        return gUnk_020000A4;
    p = sub_08219FBC(0xC, 0x200);
    if (p != 0) {
        sub_0821A04C(p, sub_08019628, sub_08019670);
        if (sub_08019698(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
