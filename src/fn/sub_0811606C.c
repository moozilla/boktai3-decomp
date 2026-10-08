#include "global.h"

extern u8 *gUnk_020001E8;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_08115FE0(u8 *);
void sub_0821A0C0(u8 *);
void sub_08115F74(void);
void sub_08115FAC(void);

u8 *sub_0811606C(void)
{
    u8 *p;
    if (gUnk_020001E8 != 0)
        return gUnk_020001E8;
    p = sub_08219FBC(0x8, 0xb98);
    if (p != 0) {
        sub_0821A04C(p, sub_08115F74, sub_08115FAC);
        if (sub_08115FE0(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
