#include "global.h"
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_0804DD48(u8 *, u32);
void sub_0821A0C0(u8 *);
void sub_0804DC40(void);
u32 sub_0804DD10(u8 *);
u8 *sub_0804DEFC(u32 a)
{
    u8 *r = sub_08219FBC(8, 0x190);
    if (r) {
        sub_0821A04C(r, sub_0804DC40, (void (*)(void))sub_0804DD10);
        if (sub_0804DD48(r, a) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
