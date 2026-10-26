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
void ProjectilePool_ReleaseSlot(struct A *, struct E *, s32);
u32 ProjectilePool_Destroy(struct A *a)
{
    struct E *e = a->e;
    s32 i = 0;
    do {
        if (a->w18 & (1 << i))
            ProjectilePool_ReleaseSlot(a, e, i);
        i++;
        e++;
    } while (i <= 0xf);
    return gProjectilePool = 0;
}
