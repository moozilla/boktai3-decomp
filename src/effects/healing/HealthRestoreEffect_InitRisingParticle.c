#include "global.h"
#include "engine/engine_globals.h"
#include "engine/ram.h"
#include "effects/healing/elem_layout.h"
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
    p->flags &= ~1;
    SpriteDrawable_SelectTileFrame(p, p->data, 6);
    table = gRandomTable;
    p->x = Mod(rnd(table), 16) + 8;
    p->y = Mod(rnd(table), 16) + 24;
    p->z = 0;
    p->dx = 0;
    p->dy = -(Mod(rnd(table), 2) + 2);
    p->dz = 0;
    p->active = 1;
    p->age = 0;
    p->update = FlickerParticle_UpdateVerticalMotion;
}
