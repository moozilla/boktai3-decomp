#include "global.h"
struct S { u8 f[0x5b8]; u32 a; u8 g[0x5b6 - 0x5bc + 0x1000]; };
struct T { u8 f[0x5b8]; u32 a; u8 g[0xe]; };
void sub_081C17D4(u8 *p, u32 v)
{
    *(u32 *)(p + 0x5b8) = v;
    *(u16 *)(p + 0x5b6) = 0;
}
