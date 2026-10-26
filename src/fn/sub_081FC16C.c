#include "global.h"
#include "ram.h"
struct E {
    u8 p[0x78];
};
struct A {
    u8 p0[0x18];
    u32 w18;
    u8 p1[0x38 - 0x1c];
    struct E e[24];
};
void Ratatosk_ReleaseAnimatedParticle(struct A *, struct E *, s32);
u32 Ratatosk_OnAnimatedParticlePoolDestroy(struct A *a)
{
    struct E *e = a->e;
    s32 i = 0;
    do {
        if (a->w18 & (1 << i))
            Ratatosk_ReleaseAnimatedParticle(a, e, i);
        i++;
        e++;
    } while (i <= 0x1F);
    return gRatatoskParticlePool = 0;
}
