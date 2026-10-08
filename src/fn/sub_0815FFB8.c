#include "global.h"

void sub_082195E0(u8 *);
void sub_08217EAC(u8 *);

s32 sub_0815FFB8(u8 *p)
{
    u8 *q;
    s32 i;
    sub_082195E0(p + 0x3c);
    q = p + 0xfc;
    i = 15;
    do {
        sub_08217EAC(q);
        q += 0x3c;
        i--;
    } while (i >= 0);
    return 0;
}
