#include "global.h"
void sub_0824923C(u8 *, u32);
void sub_08130AFC(u8 *p)
{
    u32 i = p[0x2a9];
    sub_0824923C(p, ((u32 *)*(u32 *)(p + 0x2a0))[i]);
}
