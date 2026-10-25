#include "global.h"

// confidence=low: emits one centered explosion and seven offset explosions.
// MATCHING_CONCESSION: register-pinning
#ifdef NONMATCHING
// fuzzy: 70.3%
// fix.py best (sweep after the inlined-insn fix)
// Remaining differences: RNG mask/register lifetimes and offset scheduling.
#define Hresvelgr_EmitImpactExplosions sub_081A4AB4
struct ImpactPosition {
    s16 x, y, z, pad;
};
struct ImpactActor {
    u8 unknown0[0x60];
    struct ImpactPosition position;
};
struct ImpactPool {
    u8 unknown0[0x5C];
    s16 parameterA, parameterB, parameterC;
};
extern struct ImpactPool *gUnk_02000244;
extern const u16 gUnk_0203B400[];
extern u32 gUnk_03005308;
void ExplosionEffect_CreateConfigured(void *, s32, s32, s32, u32, s32, s32, s32);
static inline s32 randomValue(const u16 *table)
{
    u32 offset;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    offset = gUnk_03005308 * 2;
    return *(const u16 *)(offset + (u32)table);
}
void Hresvelgr_EmitImpactExplosions(struct ImpactActor *actor)
{
    struct ImpactPool *pool = gUnk_02000244;
    struct ImpactPosition position;
    s32 i;
    struct ImpactPosition *p = &position;
    for (i = 0; i < 8; i++) {
        *p = actor->position;
        if (i > 0) {
            const u16 *table = gUnk_0203B400;
            s32 dx = ((randomValue(table) >> 3) % 512) - 256;
            s32 dz;
            asm("" : "+r"(dx));
            p->x += dx;
            do {
                dz = ((randomValue(table) >> 3) & 512) - 256;
            } while (0);
            asm("" : "+r"(dz));
            {
                typeof(p->z) v1 = p->z + dz;
                p->z = v1;
            }
        }
        ExplosionEffect_CreateConfigured(p, 16, pool->parameterA, pool->parameterB, 0x40000,
                                         pool->parameterC, 0, i << 3);
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081A4AB4);
#endif
