#include "global.h"

struct S080FBEDC {
    u8 filler[0x3d0];
    u8 *q;
};

void Axe_TickAuxiliarySpriteTimers(struct S080FBEDC *s)
{
    u8 *q = s->q;
    u32 *flags = (u32 *)(q + 0x708);
    u8 *p = q;
    u32 off = 0x764;
    s32 i = 3;
    do {
        u16 *timer = (u16 *)(p + off);
        if (*timer != 0) {
            if (--*timer == 0) {
                {
                    u32 f = *flags;
                    u32 one = 1;
                    asm volatile("" : "+r"(one));
                    *flags = f | one;
                }
            }
        }
        flags = (u32 *)((u8 *)flags + 0x60);
        p += 0x60;
    } while (--i >= 0);
}
