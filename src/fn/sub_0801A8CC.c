#include "global.h"
void sub_082195E0(u8 *);
void sub_0801A8CC(u8 *p)
{
    u8 *e = p + 0x674;
    s32 i = 0x12;
    do {
        sub_082195E0(e);
        e += 0x60;
        i--;
    } while (i >= 0);
    sub_082195E0(p + 0x614);
}
