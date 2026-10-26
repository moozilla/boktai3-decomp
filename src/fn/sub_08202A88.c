#include "global.h"
#include "ram.h"
struct E {
    u8 p[0x128];
};
struct A {
    u8 p0[0x18];
    u32 w18;
    u8 p1[0x1c - 0x1c];
    struct E e[16];
};
void TimedSpriteEffect_InitSlot(struct A *, struct E *, s32);
u32 ProjectilePool_Init(struct A *a)
{
    struct E *e;
    s32 i;
    gProjectilePool = a;
    a->w18 = 0;
    e = a->e;
    i = 0;
    do {
        TimedSpriteEffect_InitSlot(a, e, i);
        i++;
        e++;
    } while (i <= 0xf);
    return 0;
}
