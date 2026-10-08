#include "global.h"
void sub_080423C8(u8 *);
void sub_081D48B0(u8 *p)
{
    u8 *q = p + 0x28;
    s32 i = 4;
    do {
        sub_080423C8(q);
        q += 0x94;
        i--;
    } while (i >= 0);
    sub_080423C8(p + 0x30c);
}
