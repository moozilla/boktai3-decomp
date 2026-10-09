#include "global.h"
u8 *sub_08219FBC(u32, u32);
void sub_0821ABA8(u32, u32);
u8 *sub_0802140C(void);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_0802BFAC(u8 *, u32);
void sub_0821A0C0(u8 *);
void sub_0802BF30(void);
void sub_0802BF88(void);
u8 *sub_0802C620(u32 a)
{
    u8 *p;
    sub_0821ABA8(0x74, 3);
    p = sub_0802140C();
    if (p != 0)
        return p;
    p = sub_08219FBC(8, 0xC68);
    if (p != 0) {
        sub_0821A04C(p, sub_0802BF30, sub_0802BF88);
        if (sub_0802BFAC(p, a) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
