#include "global.h"

u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_0819A5AC(u8 *);
void sub_0821A0C0(u8 *);
void sub_0819A564(void);
void sub_0819A59C(void);

u8 *sub_0819A65C(void)
{
    u8 *p;
    p = sub_08219FBC(8, 0x7C);
    if (p != 0) {
        sub_0821A04C(p, sub_0819A564, sub_0819A59C);
        if (sub_0819A5AC(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
