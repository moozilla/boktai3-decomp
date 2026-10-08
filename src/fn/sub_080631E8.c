#include "global.h"

s32 sub_0824923C(u8 *, u32);
void sub_080629E0(u8 *);
void sub_0822B2F8(s32);
void sub_082279A8(s32, s32, s32, s32, s32, s32, s32);
void sub_0805F784(u8 *, s32 (*)(u8 *));
s32 sub_08063248(u8 *);

s32 sub_080631E8(u8 *p)
{
    if (sub_0824923C(p, *(u32 *)(p + 0x1CFC)) == 0) {
        sub_080629E0(p);
    } else {
        sub_0822B2F8(0x119);
        sub_082279A8(3, 5, 4, 4, 4, 0xFFFF, 0);
        sub_0805F784(p, sub_08063248);
    }
    return 0;
}
