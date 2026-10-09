#include "global.h"
void sub_08042588(u8 *, s32, s32, s32, u8 *);
u32 sub_080427E0(u8 *);
void sub_0802CED0(u8 *p, s32 a, s32 b, s32 c)
{
    u8 *q = p + 0x184;
    sub_08042588(q, a, b, c, p + 0xC);
    *(u32 *)(p + 0x68) = sub_080427E0(q);
}
