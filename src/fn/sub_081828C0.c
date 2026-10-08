#include "global.h"
void sub_081826C8(u8 *, u32, u32);
void sub_08182548(u8 *);
void sub_081828C0(u8 *p)
{
    sub_081826C8(p, 10, 0x40);
    if ((p[0xB42] & 0x1f) == 0)
        sub_08182548(p);
}
