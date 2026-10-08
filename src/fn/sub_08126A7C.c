#include "global.h"
void sub_08214514(u8 *);
void sub_08219DD8(u8 *, u32);
void sub_08126A7C(u32 a, u8 *p)
{
    if (p[4] != 0)
        sub_08214514(p);
    p[0x3c] = 0;
    sub_08219DD8(p, 0x40);
}
