#include "global.h"

void Effect_Remove(u8 *);

void sub_08236DA8(u8 *p)
{
    u8 *q = p + 0xA4;
    s32 i;

    for (i = 7; i >= 0; i--) {
        Effect_Remove(q);
        q += 0x3C;
    }
}
