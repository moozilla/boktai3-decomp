// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "engine.h"
#include "ram.h"

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
extern const s16 gUnk_086149C4[];
void FlickerParticle_UpdateSecondaryMotion(struct FlickerParticle *);

static inline s32 NextRandom(const u16 *table, s32 mask)
{
    gRandomIndex = (gRandomIndex + 1) & mask;
    return table[gRandomIndex];
}
static inline s32 Scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -(-value >> 12);
}

void FlickerParticle_StartSecondaryBurst(struct FlickerParticlePool *s)
{
    s32 i = 0;
    register s32 randomMask asm("r10") = 1023;
    register const s16 *sine asm("r9") = gUnk_086149C4;
    struct FlickerParticlePool *cursor = s;
    u32 offset = 0;
    register const u16 *table asm("r8") = gRandomTable;
    for (i = 0; i < 8; i++) {
        s32 angle, radius;
        cursor->particles[0].x = s->x + 8;
        cursor->particles[0].y = s->y + 8;
        angle = i * 32 + ((NextRandom(table, randomMask) >> 3) & 15) - 8;
        radius = ((NextRandom(table, randomMask) >> 3) & 3) + 4;
        {
            s32 cosineAngle = angle + 64;
            register s32 angleMask asm("r0") = 255;
            cosineAngle &= angleMask;
            cursor->particles[0].velocityX = Scale(sine[cosineAngle] * radius);
        }
        cursor->particles[0].velocityY = Scale(sine[angle & 255] * radius);
        {
            u8 *base = (u8 *)s->particles;
            struct FlickerParticle *particle;
            asm volatile("" : "+r"(base));
            particle = (struct FlickerParticle *)(base + offset);
            particle->flags &= ~1;
        }
        cursor->particles[0].active = 1;
        cursor->particles[0].age = 0;
        {
            u8 *base = (u8 *)&s->particles[0].callback;
            asm volatile("" : "+r"(base));
            *(void (**)(struct FlickerParticle *))(base + offset) =
                FlickerParticle_UpdateSecondaryMotion;
        }
        cursor = (struct FlickerParticlePool *)((u8 *)cursor + sizeof(struct FlickerParticle));
        offset += sizeof(struct FlickerParticle);
    }
}
