#include "effects/orbits/converging_effects.h"
#include "engine/engine_globals.h"
#include "engine/ram.h"

/* Tentative role: periodically spawn an inward-moving effect in a four-slot ring. */
extern const s16 gUnk_086149C4[];
void SpriteDrawable_SelectTileFrame(struct ConvergingParticle *, void *, s32);

static inline s32 random_value(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return *(u16 *)((u8 *)gRandomTable + (gRandomIndex << 1));
}
static inline s32 scale(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
void Player_EmitConvergingParticle(struct Player *p, struct SceneVec *position)
{
    if ((++p->convergingEffects.ticks & 3) == 0) {
        struct ConvergingParticle *e = &p->convergingEffects.particles[p->convergingEffects.next];
        struct ConvergingParticle *draw = e;
        s32 angle;
        s32 radius;
        u16 radiusSample;
        draw->flags &= ~1;
        e->kind = 4;
        SpriteDrawable_SelectTileFrame(draw, p->convergingEffects.graphics, 4);
        angle = ParticleRandom_ReadDirectSample(gRandomTable) & 255;
        radiusSample = ParticleRandom_ReadDirectSample(gRandomTable);
        radius = radiusSample % 128 + 256;
        e->offsetX = scale(radius * gUnk_086149C4[(angle + 64) & 255]);
        e->offsetZ = scale(radius * gUnk_086149C4[angle]);
        draw->position = *position;
        draw->position.x += (u16)e->offsetX;
        draw->position.z += (u16)e->offsetZ;
        e->speed = (random_value() & 7) + 28;
        e->ticks = 0;
        e->active = 1;
        p->convergingEffects.active = 1;
        if (++p->convergingEffects.next > 3)
            p->convergingEffects.next = 0;
    }
}
