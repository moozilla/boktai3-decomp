#include "global.h"

struct S {
    u8 f[0x4c];
    s32 a;
};
void Actor_SubtractHealth(struct S *p, s32 d)
{
    p->a -= d;
    if (p->a < 0)
        p->a = 0;
}
