#include "global.h"
void sub_082195E0(u8 *);
void sub_08214514(u8 *);
s32 sub_081BBDA8(u8 *p)
{
    u8 *q = p + 0x58;
    s32 i = 0xf;
    do {
        sub_082195E0(q);
        q += 0x60;
        i--;
    } while (i >= 0);
    q = p + 0x658;
    i = 1;
    do {
        sub_08214514(q);
        q += 0x2c;
        i--;
    } while (i >= 0);
    return 0;
}
