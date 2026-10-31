#include "global.h"
#include "engine.h"
#include "ram.h"
#include "contact_position.h"

// confidence=low: emits one centered explosion and seven offset explosions.
#ifdef NONMATCHING
// fuzzy: 70.0%
#define Hresvelgr_EmitImpactExplosions sub_081A4AB4
struct HresvelgrImpactProjectile {
    u8 unknown0[0x60];
    struct ContactPosition position;
};
struct HresvelgrImpactPool {
    u8 unknown0[0x5C];
    s16 parameterA, parameterB, parameterC;
};
void ExplosionEffect_CreateConfigured(void *, s32, s32, s32, u32, s32, s32, s32);
static inline s32 Hresvelgr_NextImpactSample(const u16 *table)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + (u32)table);
}
void Hresvelgr_EmitImpactExplosions(struct HresvelgrImpactProjectile *actor)
{
    struct HresvelgrImpactPool *pool = gHresvelgrProjectilePool;
    struct ContactPosition position;
    s32 i;
    for (i = 0; i < 8; i++) {
        struct ContactPosition *p = &position;
        position = actor->position;
        if (i > 0) {
            const u16 *table = gRandomTable;
            s32 dx = ((Hresvelgr_NextImpactSample(table) >> 3) % 512) - 256;
            s32 dz;
            p->x += dx;
            dz = Hresvelgr_NextImpactSample(table) >> 3;
            p->z += ((u16)dz & 512) - 256;
        }
        ExplosionEffect_CreateConfigured(&position, 16, pool->parameterA, pool->parameterB, 0x40000,
                                         pool->parameterC, 0, i << 3);
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081A4AB4);
#endif
