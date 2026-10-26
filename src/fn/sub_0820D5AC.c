#include "global.h"
#include "ram.h"
struct E {
    u8 p[0xB0];
};
struct A {
    u8 p0[0x18];
    u32 w18;
    u8 p1[0x1C - 0x1c];
    struct E e[24];
};
void EnemyParticle_ReleaseSlot(struct A *, struct E *, s32);
u32 EnemyParticle_DestroyPool(struct A *a)
{
    struct E *e = a->e;
    s32 i = 0;
    do {
        if (a->w18 & (1 << i))
            EnemyParticle_ReleaseSlot(a, e, i);
        i++;
        e++;
    } while (i <= 0xF);
    return gEnemyParticlePool = 0;
}
