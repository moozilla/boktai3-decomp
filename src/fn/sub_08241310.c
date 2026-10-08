#include "global.h"

struct P08241310 { u8 f0[0x4F6]; u8 f4F6; u8 f4F7[0x5B9 - 0x4F7]; u8 f5B9; u8 f5BA; u8 f5BB; };

s32 sub_08241310(struct P08241310 *p)
{
    s32 v = 12;
    if (p->f5B9 == 2 && p->f5BB != 0)
        v = 8;
    if (p->f4F6 != 0)
        v -= p->f4F6;
    if (v <= 1)
        return 2;
    return v;
}
