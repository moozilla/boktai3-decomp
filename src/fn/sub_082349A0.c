#include "global.h"
#include "engine_globals.h"
#include "ram.h"
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
void SpriteDrawable_SelectTileFrame(struct Elem *, void *, u32);
void FlickerParticle_UpdateVerticalMotion(struct Elem *);
s32 Mod(s32, s32);
static inline s32 rnd(u16 *table)
{
    s32 v;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    v = table[gRandomIndex];
    return v >> 3;
}
void HealthRestoreEffect_InitRisingParticle(struct Elem *p)
{
    u16 *table;
    u32 zero;
    p->flags &= ~1;
    SpriteDrawable_SelectTileFrame(p, p->data, 6);
    table = gRandomTable;
    p->x = Mod(rnd(table), 16) + 8;
    zero = 0;
    p->y = Mod(rnd(table), 16) + 24;
    p->z = zero;
    p->dx = zero;
    p->dy = -(Mod(rnd(table), 2) + 2);
    p->dz = zero;
    p->active = 1;
    p->age = zero;
    p->update = FlickerParticle_UpdateVerticalMotion;
}
