#include "global.h"
#include "player.h"
struct S {
    u8 filler[0x3d0];
    u8 *q;
};
static inline u8 TestFlags(u32 flags, u32 mask)
{
    if (flags & mask)
        return TRUE;
    return FALSE;
}
void Bee_ConsumeFlag80(struct S *s)
{
    u8 *q = s->q;
    u32 mask = 0x80;
    u32 off = 0x558;
    if (TestFlags(*(u32 *)(q + off), mask)) {
        u8 *p = (*(u8 **)gPlayers) + 0x4F6;
        *p = *p - 3;
        {
            u32 off2 = 0x558;
            u32 *f = (u32 *)(s->q + off2);
            u32 m = ~0x80;
            *f = *f & m;
        }
    }
}
