#include "global.h"

struct S {
    u8 f[0x114];
    u32 a;
    u32 b;
};
void Actor_SetLifecycleCallbacks(struct S *p, u32 a, u32 b)
{
    p->a = a;
    p->b = b;
}
