#include "global.h"
void sub_08013220(u8 *, u8 *);
u32 sub_0801324C(u8 *p)
{
    u8 *q = p + 0x5c;
    s32 i = 3;
    do {
        sub_08013220(p, q);
        i--;
        q += 0xa8;
    } while (i >= 0);
    return 0;
}
