#include "global.h"
#include "engine.h"
#include "ram.h"
#include "monster_particle_random.h"

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

#define FLICKER_CAPTURE_LOOKUP(out_, value_)                                                       \
    do {                                                                                           \
        (out_) = (value_);                                                                         \
    } while (0)

static inline s32 NextRandom(const u16 *table, s32 mask)
{
    s32 sample;
    gRandomIndex = (gRandomIndex + 1) & mask;
    PARTICLE_RANDOM_READ_SAMPLE(sample, table, gRandomIndex);
    return sample;
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
    s32 randomMask;
    const s16 *sine;
    struct FlickerParticlePool *cursor;
    u32 offset;
    const u16 *table;

    FLICKER_CAPTURE_LOOKUP(randomMask, 1023);
    FLICKER_CAPTURE_LOOKUP(sine, gUnk_086149C4);
    cursor = s;
    offset = 0;
    table = gRandomTable;
    for (; i < 8; i++) {
        s32 angle, radius;
        union {
            s32 sample;
            u16 phaseMask;
            s16 amplitude;
        } polar;
        cursor->particles[0].x = s->x + 8;
        cursor->particles[0].y = s->y + 8;
        angle = i * 32 + ((NextRandom(table, randomMask) >> 3) & 15) - 8;
        polar.sample = NextRandom(table, randomMask);
        radius = ((polar.sample >> 3) & 3) + 4;
        {
            s32 cosineAngle = angle + 64;
            FLICKER_CAPTURE_LOOKUP(polar.phaseMask, 255);
            cosineAngle &= polar.phaseMask;
            polar.amplitude = sine[cosineAngle];
            cursor->particles[0].velocityX = Scale(polar.amplitude * radius);
        }
        cursor->particles[0].velocityY = Scale(sine[angle & 255] * radius);
        {
            u8 *base = (u8 *)s->particles;
            struct FlickerParticle *particle;
            particle = (struct FlickerParticle *)(base + offset);
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
