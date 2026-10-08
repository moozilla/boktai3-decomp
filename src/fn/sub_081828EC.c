#include "global.h"
void sub_081826C8(u8 *, u32, u32);
void sub_081825D0(u8 *);
void sub_081828EC(u8 *p)
{
    sub_081826C8(p, 15, 0x2A);
    if ((p[0xB42] & 0xF) == 0)
        sub_081825D0(p);
}
