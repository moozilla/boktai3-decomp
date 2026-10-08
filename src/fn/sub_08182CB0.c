#include "global.h"
void sub_08182B14(u8 *, u32, u32);
void sub_08182548(u8 *);
void sub_08182CB0(u8 *p)
{
    sub_08182B14(p, 7, 0x40);
    if ((p[0xB42] & 0x1f) == 0)
        sub_08182548(p);
}
