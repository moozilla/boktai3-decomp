#include "global.h"

void sub_08217EAC(u8 *);

void sub_08236DA8(u8 *p)
{
    u8 *q = p + 0xA4;
    s32 i;

    for (i = 7; i >= 0; i--) {
        sub_08217EAC(q);
        q += 0x3C;
    }
}
