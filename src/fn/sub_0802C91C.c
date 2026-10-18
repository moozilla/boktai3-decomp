#include "global.h"

struct S {
    u8 f[0x148];
    u32 a;
};
void Actor_SetHealth(struct S *, u32);
void Actor_SetHealthLimits(struct S *p, u32 a, u32 b)
{
    p->a = a;
    Actor_SetHealth(p, b);
}
