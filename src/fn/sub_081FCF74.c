#include "global.h"
#include "ram.h"
struct E {
    u8 p[0x1c0];
};
struct A {
    u8 p0[0x18];
    u32 w18;
    u8 p1[0x20 - 0x1c];
    struct E e[10];
};
void SceneProjectile_InitSlot(struct A *, struct E *, s32);
u32 SceneProjectile_InitPool(struct A *a)
{
    struct E *e;
    s32 i;
    gSceneProjectilePool = a;
    a->w18 = 0;
    e = a->e;
    i = 0;
    do {
        SceneProjectile_InitSlot(a, e, i);
        i++;
        e++;
    } while (i <= 9);
    return 0;
}
