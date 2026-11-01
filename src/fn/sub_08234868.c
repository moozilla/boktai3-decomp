#include "global.h"
#include "engine.h"
#include "ram.h"

struct FlickerMotion {
    u16 x, y;
    u8 unk_04[0x0C];
    u16 age, active;
    s16 velocityX, velocityY;
    u8 unk_18[4];
    void *resource;
};
struct FlickerParticle {
    u32 flags;
    u8 unk_04[0x14];
    struct FlickerMotion motion;
    void (*update)(struct FlickerParticle *);
};
struct HealthRestoreEffect {
    u8 unk_00[0x8C];
    struct FlickerParticle particles[8];
};
extern const s16 gUnk_086149C4[];
void FlickerParticle_UpdateMotion(struct FlickerParticle *);

static inline s32 NextRandom(const u16 *table, u32 *index, u32 mask)
{
    *index = (*index + 1) & mask;
    return table[*index];
}
static inline s32 Scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}

#ifdef NONMATCHING
// fuzzy: 84.4%
// scope: 17 ROM / 13 draft insns differ, shape 10. Exact with AGBCC_NOEQUIV=29
// (the random table gets no REG_EQUIV); see notes/opus-health-restore-burst_2026-11-01.md
// Initialize the eight outward-moving particles used by the health restore effect.
void HealthRestoreEffect_StartParticleBurst(struct HealthRestoreEffect *effect)
{
    s32 i = 0;
    u32 *index = &gRandomIndex;
    u32 mask = 0x3FF;
    const s16 *sine = gUnk_086149C4;
    u32 offset = 0;
    struct FlickerMotion *motion = &effect->particles[0].motion;
    const u16 *table = gRandomTable;
    u32 byteMask = 255;

    for (; i < 8; i++) {
        s32 angle, speed;
        motion->x = 16;
        motion->y = 32;
        angle = i * 32 + ((NextRandom(table, index, mask) >> 3) & 15) - 8;
        speed = ((NextRandom(table, index, mask) >> 3) & 3) + 4;
        motion->velocityX = Scale(sine[(angle + 64) & 255] * speed);
        byteMask = 255;
        motion->velocityY = Scale(sine[angle & byteMask] * speed);
        {
            u8 *base = (u8 *)effect->particles;
            struct FlickerParticle *particle = (struct FlickerParticle *)(base + offset);
            particle->flags &= ~1;
        }
        motion->active = 1;
        motion->age = 0;
        {
            u8 *base = (u8 *)&effect->particles[0].update;
            *(void (**)(struct FlickerParticle *))(base + offset) = FlickerParticle_UpdateMotion;
        }
        offset += sizeof(struct FlickerParticle);
        motion = (struct FlickerMotion *)((u8 *)motion + sizeof(struct FlickerParticle));
    }
}
#else
INCLUDE_ASM("asm/nonmatching", HealthRestoreEffect_StartParticleBurst);
#endif
