#include "global.h"
void sub_08214514(u8 *);
void sub_0820D2A0(u32 *a, u32 *b, u32 n)
{
    b[0x7C / 4] |= 1;
    sub_08214514((u8 *)b + 0x7C);
    a[6] &= ~(1 << n);
}
