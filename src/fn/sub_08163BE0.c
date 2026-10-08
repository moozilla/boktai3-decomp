#include "global.h"

void sub_08217EAC(u8 *);

s32 sub_08163BE0(u8 *p)
{
    u8 *q = p + 0x74;
    s32 i = 15;
    do {
        sub_08217EAC(q);
        q += 0x3c;
        i--;
    } while (i >= 0);
    return 0;
}
