#include "engine/shared/global.h"
struct S {
    u8 f[0x8E4];
    u16 a[24];
};
void StageResults_InitAnimationIds(struct S *s)
{
    s32 i;
    s->a[0] = 0x34;
    s->a[1] = 0x39;
    s->a[2] = 0x3a;
    s->a[3] = 0x3b;
    s->a[6] = 0x48;
    s->a[4] = 0x21;
    s->a[5] = 0x22;
    s->a[7] = 0x3c;
    for (i = 8; i < 23; i++)
        s->a[i] = 0x3e;
}
