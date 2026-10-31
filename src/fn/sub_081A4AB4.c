#include "global.h"
#include "engine.h"
#include "ram.h"
#include "contact_position.h"
#include "monster_particle_random.h"

// confidence=low: emits one centered explosion and seven offset explosions.
#define Hresvelgr_EmitImpactExplosions sub_081A4AB4
struct HresvelgrImpactProjectile {
    u8 unknown0[0x60];
    struct ContactPosition position;
};
struct HresvelgrImpactPool {
    u8 unknown0[0x5C];
    s16 parameterA, parameterB, parameterC;
};
u8 *ExplosionEffect_CreateConfigured(void *, u32, s32, s32, s32, s32, s32, u32);
void Hresvelgr_EmitImpactExplosions(struct HresvelgrImpactProjectile *actor)
{
    struct HresvelgrImpactPool *pool = gHresvelgrProjectilePool;
    struct ContactPosition position;
    s32 i;
    for (i = 0; i < 8; i++) {
        struct ContactPosition *p = &position;
        s32 minimumOffset = -256;
        position = actor->position;
        if (i > 0) {
            const u16 *table = gRandomTable;
            u32 *index = &gRandomIndex;
            s32 sample, dx, dz;
            *index = (*index + 1) & 1023;
            PARTICLE_RANDOM_READ_SAMPLE(sample, table, *index);
            dx = ((sample >> 3) % 512) + minimumOffset;
            p->x += dx;
            *index = (*index + 1) & 1023;
            PARTICLE_RANDOM_READ_SAMPLE(dz, table, *index);
            dz >>= 3;
            p->z += ((u16)dz & 512) + minimumOffset;
        }
        ExplosionEffect_CreateConfigured(&position, 16, pool->parameterA, pool->parameterB, 0x40000,
                                         pool->parameterC, 0, i << 3);
    }
}
