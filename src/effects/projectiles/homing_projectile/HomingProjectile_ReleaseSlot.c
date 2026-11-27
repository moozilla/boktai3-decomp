#include "global.h"
struct A {
    u8 p0[0x18];
    u32 w18;
    u8 p1[0x43c - 0x1c];
    s32 cnt;
};
struct H {
    u8 f[6];
    u16 h;
};
void Sprite_Remove(void *);
void HomingProjectile_ReleaseSlot(struct A *a, u8 *b, u32 n)
{
    u32 t;
    u32 *q;
    struct H *s;
    a->cnt--;
    s = (struct H *)(b + 0x78);
    s->h = 4 | s->h;
    q = (u32 *)(b + 0x10c);
    t = *q;
    *q = t | 1;
    Sprite_Remove(q);
    a->w18 &= ~(1 << n);
}
