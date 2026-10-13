#include "global.h"
void Effect_Remove(u8 *);
void Memory_Clear(u8 *, u32);
void sub_0813AA9C(u8 *a, u8 *b)
{
    u32 z;
    Effect_Remove(a + (((s8)b[0x17]) * 0x28 + 0x1c));
    z = 0;
    b[0x16] = z;
    *(u32 *)(b + 0x20) = z;
    Memory_Clear(b, 0x24);
    b[0x17] = 0xff;
}
