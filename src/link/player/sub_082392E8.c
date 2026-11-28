#include "effects/orbits/converging_effects.h"
#include "scene/areas/area_effect_link.h"
#include "engine/engine_globals.h"
#include "engine/ram.h"

extern const s16 gUnk_086149C4[];
void SpriteDrawable_SelectTileFrame(struct ConvergingParticle *, void *, s32);

static inline s32 ReadParticleSpeedSample(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return *(u16 *)((u8 *)RANDOM_TABLE_ADDR + (gRandomIndex << 1));
}
static inline s32 scale(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
static inline u16 linkValue(s32 index)
{
    struct AreaEffectLinkState *link = gLinkSession;
    if (!link || index < 0)
        return 0;
    return link->playerValues[index];
}
// confidence=low: spawns a converging particle at a rate scaled by the link player's value.
void sub_082392E8(struct Player *p, struct SceneVec *position)
{
    u32 step = linkValue(p->slot);
    p->convergingEffects.ticks += step + 2;
    if (p->convergingEffects.ticks >= 40) {
        struct ConvergingParticle *e = &p->convergingEffects.particles[p->convergingEffects.next];
        struct ConvergingParticle *draw = e;
        s32 angle;
        s32 radius;
        draw->flags &= ~1;
        e->kind = 2;
        SpriteDrawable_SelectTileFrame(draw, p->convergingEffects.graphics, 2);
        angle = ParticleRandom_ReadSample((const u16 *)gRandomTable) & 255;
        // The single-pass loop is needed for the ROM register allocation.
        do {
            radius = (ParticleRandom_ReadDirectSample(gRandomTable) & 63) + 128;
        } while (0);
        e->offsetX = scale(radius * gUnk_086149C4[(angle + 64) & 255]);
        e->offsetZ = scale(radius * gUnk_086149C4[angle]);
        draw->position = *position;
        draw->position.x += e->offsetX;
        draw->position.z += e->offsetZ;
        e->speed = (ReadParticleSpeedSample() & 3) + 14;
        e->ticks = 0;
        e->active = 1;
        if (++p->convergingEffects.next > 3)
            p->convergingEffects.next = 0;
        p->convergingEffects.ticks -= 40;
        p->convergingEffects.active = 1;
    }
}
