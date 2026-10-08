#include "global.h"
struct P { u8 f0[0x4D9C]; s16 a; s16 b; };
void sub_08165C08(u8 *, u8 *, s32);
void sub_08166714(struct P *p, s32 n)
{
    if (p->a >= 0)
        sub_08165C08((u8 *)p + 0x4200, (u8 *)p + 0x442C, n);
    if (p->b >= 0)
        sub_08165C08((u8 *)p + 0x4260, (u8 *)p + 0x4434, n);
}
