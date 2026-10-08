#include "global.h"

void sub_0814B1E0(u8 *p)
{
    u8 *q = p + 0xAC;
    u32 a = *(u32 *)(p + 0x30);
    u32 b = *(u32 *)(p + 0x34);

    *(u32 *)q = a;
    *(u32 *)(q + 4) = b;
    p += 0xEC;
    *(u32 *)(p + 8) |= 1;
}
