#include "global.h"
void sub_082195E0(u8 *);
void sub_08214514(u8 *);
s32 sub_081BD2D4(u8 *p)
{
    u8 *q = p + 0x58;
    s32 i = 0x15;
    do {
        sub_082195E0(q);
        q += 0x60;
        i--;
    } while (i >= 0);
    sub_08214514(p + 0x898);
    return 0;
}
