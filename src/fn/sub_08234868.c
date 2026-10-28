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
// fuzzy: 82.8%
// scope shape 12 (differences once low registers are set aside)
// Initialize the eight outward-moving particles used by the health restore effect.
void HealthRestoreEffect_StartParticleBurst(struct HealthRestoreEffect *effect)
{
    const u16 *table = gRandomTable;
    s32 i = 0;
    u32 *index = &gRandomIndex;
    u32 mask = 0x3FF;
    const s16 *sine = gUnk_086149C4;
    struct FlickerMotion *motion = &effect->particles[0].motion;
    for (; i < 8; i++) {
        s32 angle, speed;
        motion->x = 16;
        motion->y = 32;
        angle = (i * 32 + ((NextRandom(table, index, mask) >> 3) & 15)) - 8;
        speed = ((NextRandom(table, index, mask) >> 3) & 3) + 4;
        motion->velocityX = Scale(sine[(angle + 64) & 255] * speed);
        motion->velocityY = Scale(sine[angle & 255] * speed);
        effect->particles[i].flags &= ~1;
        motion->active = 1;
        motion->age = 0;
        effect->particles[i].update = FlickerParticle_UpdateMotion;
        motion = (struct FlickerMotion *)((u8 *)motion + sizeof(struct FlickerParticle));
    }
}
#else
INCLUDE_ASM("asm/nonmatching", HealthRestoreEffect_StartParticleBurst);
#endif
