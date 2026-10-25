// MATCHING_CONCESSION: register-pinning
#include "global.h"

struct FlickerParticle {
    u32 flags;
    u8 unk_04[0x18 - 4];
    u16 x, y;
    u8 unk_1C[0x28 - 0x1C];
    u16 age, active, velocityX, velocityY;
    u8 unk_30[4];
    void *resource;
    void (*callback)(struct FlickerParticle *);
};
struct FlickerParticlePool {
    u8 unk_00[0x7C];
    u16 x, y;
    u8 unk_80[0x168 - 0x80];
    struct FlickerParticle particles[8];
};
extern u32 gUnk_03005308;
extern const u16 gUnk_0203B400[];
extern const s16 gUnk_086149C4[];
void FlickerParticle_UpdateSecondaryMotion(struct FlickerParticle *);

static inline s32 NextRandom(const u16 *table, s32 mask)
{
    gUnk_03005308 = (gUnk_03005308 + 1) & mask;
    return table[gUnk_03005308];
}
static inline s32 Scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -(-value >> 12);
}

#ifdef NONMATCHING
// fuzzy: 98.6%
void FlickerParticle_StartSecondaryBurst(struct FlickerParticlePool *s)
{
    s32 i = 0;
    register s32 randomMask asm("r10") = 1023;
    register const s16 *sine asm("r9") = gUnk_086149C4;
    struct FlickerParticlePool *cursor = s;
    u32 offset = 0;
    register const u16 *table asm("r8") = gUnk_0203B400;
    for (i = 0; i < 8; i++) {
        s32 angle, radius;
        cursor->particles[0].x = s->x + 8;
        cursor->particles[0].y = s->y + 8;
        angle = i * 32 + ((NextRandom(table, randomMask) >> 3) & 15) - 8;
        radius = ((NextRandom(table, randomMask) >> 3) & 3) + 4;
        cursor->particles[0].velocityX = Scale(sine[(angle + 64) & 255] * radius);
        cursor->particles[0].velocityY = Scale(sine[angle & 255] * radius);
        {
            u8 *base = (u8 *)s->particles;
            struct FlickerParticle *particle = (struct FlickerParticle *)(base + offset);
            particle->flags &= ~1;
        }
        cursor->particles[0].active = 1;
        cursor->particles[0].age = 0;
        {
            u8 *base = (u8 *)&s->particles[0].callback;
            *(void (**)(struct FlickerParticle *))(base + offset) =
                FlickerParticle_UpdateSecondaryMotion;
        }
        cursor = (struct FlickerParticlePool *)((u8 *)cursor + sizeof(struct FlickerParticle));
        offset += sizeof(struct FlickerParticle);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", FlickerParticle_StartSecondaryBurst);
#endif
