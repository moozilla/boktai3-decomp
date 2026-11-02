#include "global.h"
#include "monster_particle_random.h"
#include "engine.h"
#include "ram.h"

#define gEffectRandomIndex gRandomIndex
struct EffectPosition {
    s16 x, y, z, pad;
};
struct EffectTrailSprite {
    struct EffectPosition position;
    u8 pad[0x24];
};
struct EffectCollider {
    u8 pad[6];
    u16 flags;
};
struct ActorEffect {
    u8 entered, state;
    u8 pad2[4];
    u16 timer;
    u8 trailIndex, direction;
    u8 pada[0x5c - 0xa];
    u8 angles[6], radii[6];
    u8 pad68[0xbc - 0x68];
    struct EffectCollider healthCollider;
    u8 padc4[0x12c - 0xc4];
    struct EffectTrailSprite trails[6];
};
struct EffectOwner;
s32 Math_Atan2(s32, s32);
void ActorEffect_UpdateRadialTrail(struct ActorEffect *);
u32 ActorEffect_RemoveSprites(struct EffectOwner *, struct ActorEffect *, u32);
void ActorEffect_UpdateRadialExpiry(struct EffectOwner *owner, struct ActorEffect *effect,
                                    u32 index)
{
    if (effect->entered) {
        s32 i;
        effect->entered = 0;
        {
            struct EffectCollider *c = &effect->healthCollider;
            c->flags |= 4;
        }
        {
            struct EffectPosition *tail = &effect->trails[1].position;
            struct EffectPosition *head = &effect->trails[0].position;
            effect->direction = Math_Atan2(head->x - tail->x, head->z - tail->z);
        }
        {
            u8 *angles;
            u8 *radii;
            union ParticleRandomTableSource source;
            const u16 *table;
            u32 *randomIndex;
            i = 0;
            angles = effect->angles;
            radii = effect->radii;
            source.halves[0] = (u16)RANDOM_TABLE_ADDR;
            source.halves[1] = RANDOM_TABLE_ADDR >> 16;
            table = ParticleRandom_GetTable(&source);
            randomIndex = &gRandomIndex;
            do {
                angles[i] =
                    (s16)((ParticleRandom_NextSample(table, randomIndex, 1023) & 31) + 240) +
                    effect->direction;
                radii[i] = (ParticleRandom_NextSample(table, randomIndex, 1023) & 15) + 24;
            } while (++i < 6);
        }
    }
    ActorEffect_UpdateRadialTrail(effect);
    if (effect->timer > 29)
        ActorEffect_RemoveSprites(owner, effect, index);
    else
        effect->timer++;
}
