#include "global.h"
void TimedHazard_OnDamageContact(u8 *a, u8 *b, u8 *c)
{
    u32 d = *(u16 *)(b + 0x3e);
    u32 m = 8;
    u16 *q;
    u32 v;
    if (*(u32 *)(a + 0x38) & m)
        d <<= 1;
    q = (u16 *)(c + 0x16);
    v = *q - d;
    *q = v;
    if ((s32)(v << 16) < 0)
        *q = 0;
}
