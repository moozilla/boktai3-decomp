#include "global.h"

struct S { u8 f[0x64]; u16 a; u8 g[0x384 - 0x66]; u8 b; };
void sub_08042798(u8 *, u32);
void sub_0802D040(u8 *p)
{
    sub_08042798(p + 0x184, 0x192);
    *(u16 *)(p + 0x64) = 10;
}
