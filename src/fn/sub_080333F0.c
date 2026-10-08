#include "global.h"

extern u8 *gUnk_020000E0;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_0803336C(u8 *);
void sub_0821A0C0(u8 *);
void sub_080332B0(void);
void sub_08033340(void);

u8 *sub_080333F0(void)
{
    u8 *p;
    if (gUnk_020000E0 != 0)
        return gUnk_020000E0;
    p = sub_08219FBC(0x3, 0x19c);
    if (p != 0) {
        sub_0821A04C(p, sub_080332B0, sub_08033340);
        if (sub_0803336C(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
