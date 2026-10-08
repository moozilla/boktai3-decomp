#include "global.h"

struct S { u8 f[0x6e]; u16 t; };

s32 sub_08057C78(struct S *p)
{
    u16 *tp = &p->t;
    u32 t;
    (*tp)++;
    if (*tp > 0x81)
        *tp = 0;
    t = *tp;
    if (t <= 0x1d)
        return 0;
    if (t <= 0x27)
        return 1;
    if (t <= 0x31)
        return 2;
    if (t <= 0x6d)
        return 3;
    if (t <= 0x77)
        return 2;
    return 1;
}
