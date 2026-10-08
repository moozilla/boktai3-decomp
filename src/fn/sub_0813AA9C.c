#include "global.h"
void sub_08217EAC(u8 *);
void sub_08219DD8(u8 *, u32);
void sub_0813AA9C(u8 *a, u8 *b)
{
    u32 z;
    sub_08217EAC(a + (((s8)b[0x17]) * 0x28 + 0x1c));
    z = 0;
    b[0x16] = z;
    *(u32 *)(b + 0x20) = z;
    sub_08219DD8(b, 0x24);
    b[0x17] = 0xff;
}
