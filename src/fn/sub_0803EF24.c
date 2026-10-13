#include "global.h"

void sub_0803F638(u8 *);

void sub_0803EF24(u8 *a, u8 *b, u32 n)
{
    u32 m;
    *(u32 *)(b + 0xb4) |= 1;
    sub_0803F638(b + 0xf0);
    m = 1 << n;
    *(u32 *)(a + 0x18) &= ~m;
}
