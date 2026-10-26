#include "global.h"
struct Elem {
    u32 flags;
    u8 pad4[0x14];
    u16 x, y, z;
    u8 pad1E[10];
    u16 age, active, dx, dy, dz;
    u16 pad32;
    void *data;
    void (*update)(struct Elem *);
};
extern u32 gRandomIndex;
extern u16 gUnk_0203B400[];
void SpriteDrawable_SelectTileFrame(struct Elem *, void *, u32);
void FlickerParticle_UpdateStationary(struct Elem *);
s32 Mod(s32, s32);
static inline s32 rnd(u16 *table)
{
    s32 v;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    v = table[gRandomIndex];
    return v >> 3;
}
void HealthRestoreEffect_InitSparkle(struct Elem *p)
{
    u16 *table;
    u32 zero;
    p->flags &= ~1;
    SpriteDrawable_SelectTileFrame(p, p->data, 2);
    table = gUnk_0203B400;
    p->x = Mod(rnd(table), 80) + 24;
    zero = 0;
    p->y = (rnd(table) & 7) + 8;
    p->z = zero;
    p->active = 1;
    p->age = zero;
    p->update = FlickerParticle_UpdateStationary;
}
