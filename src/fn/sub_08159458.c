#include "global.h"

struct P08159458 { u8 f0[0x1c]; u32 f1c; u8 f20[0x457 - 0x20]; u8 f457; u8 f458; };

s32 sub_08159458(struct P08159458 *p)
{
    u8 k;
    if (!(p->f1c & 1))
        return 0;
    k = p->f457;
    if (k <= 1) goto one;
    if (k == 3) goto one;
    if (k == 7 && p->f458 == 0) goto one;
    return 0;
one:
    return 1;
}
