#include "global.h"
void sub_08182918(u8 *, u32, u32);
void sub_081825D0(u8 *);
void sub_08182AE8(u8 *p)
{
    sub_08182918(p, 20, 0x20);
    if ((p[0xB42] & 0x7) == 0)
        sub_081825D0(p);
}
