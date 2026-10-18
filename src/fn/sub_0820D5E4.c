#include "global.h"
struct E {
    u8 p[0xB0];
};
struct A {
    u8 p0[0x18];
    u32 w18;
    u8 p1[0x1c - 0x1c];
    struct E e[32];
};
extern struct A *gUnk_02000604;
void SceneOverlayEntry_Init(struct A *, struct E *, s32);
u32 EnemyParticle_InitPool(struct A *a)
{
    struct E *e;
    s32 i;
    gUnk_02000604 = a;
    a->w18 = 0;
    e = a->e;
    i = 0;
    do {
        SceneOverlayEntry_Init(a, e, i);
        i++;
        e++;
    } while (i <= 0xF);
    return 0;
}
