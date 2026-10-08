#include "global.h"

void sub_0805F2CC(u8 *, u8 *);
s32 sub_0805E1D0(u8 *, u8 *, u8 *, u8 *);
void sub_0822B2F8(s32);
void sub_08061AF0(u8 *);
void sub_08061C04(u8 *);

s32 sub_080627C4(u8 *p)
{
    s32 r;
    sub_0805F2CC(p, p + 0x1D00);
    r = sub_0805E1D0(p + 0x1C8, p + 0x20, p + 0x1AAA, p + 0x1AAF);
    if (r == 1) {
        sub_08061AF0(p);
    } else if (r == 0) {
        sub_0822B2F8(0xde);
        sub_08061C04(p);
    }
    return 0;
}
