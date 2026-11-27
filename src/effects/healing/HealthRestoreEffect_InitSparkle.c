#include "global.h"
#include "engine/engine_globals.h"
#include "engine/ram.h"
#include "effects/healing/elem_layout.h"
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
    p->flags &= ~1;
    SpriteDrawable_SelectTileFrame(p, p->data, 2);
    table = gRandomTable;
    p->x = Mod(rnd(table), 80) + 24;
    p->y = (rnd(table) & 7) + 8;
    p->z = 0;
    p->active = 1;
    p->age = 0;
    p->update = FlickerParticle_UpdateStationary;
}
