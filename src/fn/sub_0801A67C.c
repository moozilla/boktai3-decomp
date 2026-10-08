#include "global.h"
void sub_082195E0(u8 *);
void sub_0801A67C(u8 *p)
{
    u8 *e = p + 0x194;
    s32 i = 2;
    do {
        sub_082195E0(e);
        e += 0x60;
        i--;
    } while (i >= 0);
}
