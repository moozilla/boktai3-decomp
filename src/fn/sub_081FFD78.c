#include "global.h"
#include "ram.h"
struct E {
    u8 p[0x160];
};
struct A {
    u8 p0[0x18];
    u32 w18;
    u8 p1[0x1c - 0x1c];
    struct E e[3];
};
void Collider_Remove(void *);
void HomingProjectile_ReleaseSlot(struct A *, struct E *, s32);
u32 HomingProjectile_DestroyManager(struct A *a)
{
    struct E *e = a->e;
    s32 i = 0;
    do {
        if (a->w18 & (1 << i)) {
            Collider_Remove((u8 *)e + 0x78);
            HomingProjectile_ReleaseSlot(a, e, i);
        }
        i++;
        e++;
    } while (i <= 2);
    return gHomingManager = 0;
}
