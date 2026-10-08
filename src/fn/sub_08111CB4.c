#include "global.h"
struct S { u8 f[0x11CC]; u32 a; u32 b; u32 c; };
u32 sub_08033690(s32, s32, s32, s32);
void sub_08111CB4(struct S *s)
{
    s->a = sub_08033690(1, 3, 0xa, 2);
    s->b = sub_08033690(0xd, 3, 4, 2);
    s->c = sub_08033690(0xd, 7, 0x10, 2);
}
