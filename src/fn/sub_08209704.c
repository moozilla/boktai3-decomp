#include "global.h"
#include "engine.h"
#include "ram.h"
#include "monster_particle_random.h"
#include "scene_vec.h"

struct FanCannon {
    s32 age;
    u8 unk_04[0x28];
    struct SceneVec position;
    u8 unk_34[0x108 - 0x34];
    s32 range;
    s16 maximumWind;
    u8 unk_10E[0x11A - 0x10E];
    s8 direction;
    u8 reverse;
    s16 period, palette, wind;
};
struct ParticleParameters {
    u32 data[14];
};
extern const s16 gUnk_086149C4[];
s32 Mod(s32, s32);
void EnemyParticle_InitParameters(void *);
void EnemyParticle_SetGraphicsParameters(void *, u32, u32, s32);
void EnemyParticle_SetMotionParameters(void *, u16, u16, u8, u32);
void EnemyParticle_AddFlags(void *, u32);
s32 EnemyParticle_Emit(void *, void *);

/* The same origin is used for the jitter and the particle-frame draw. */
static inline u32 FanCannon_SampleOrigin(void)
{
    return (u32)gRandomTable + (PARTICLE_RANDOM_SAMPLE_ORIGIN - RANDOM_TABLE_ADDR);
}

static inline s32 FanCannon_RandomRange(s32 range, const u16 *samples)
{
    s32 result = 0;
    if (range)
        result = Mod(ParticleRandom_ReadDirectSample(samples), range);
    return result;
}

static inline s32 FanCannon_Scale(s32 value)
{
    return value >= 0 ? value >> 12 : -((-value) >> 12);
}

static inline void FanCannon_Polar(struct SceneVec *out, u8 angle, s32 radius)
{
    s16 cosine, sine;
    cosine = gUnk_086149C4[(angle + 64) & 255];
    out->x = FanCannon_Scale(radius * cosine);
    out->y = 0;
    sine = gUnk_086149C4[angle];
    out->z = FanCannon_Scale(sine * radius);
}

static inline void FanCannon_AddPosition(struct SceneVec *out, const struct SceneVec *a,
                                         const struct SceneVec *b)
{
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
}

u32 FanCannon_UpdateActive(struct FanCannon *p)
{
    struct ParticleParameters parameters;
    struct SceneVec position, velocity, offset;
    struct SceneVec *pos;
    u8 baseAngle;
    u8 angle;
    s32 frame, distance, half, span;
    const void *source;
    if (Mod(p->age, 30) == 0) {
        position = p->position;
        pos = &position;
        if (p->direction == 0) {
            s16 sample;
            s32 value;
            baseAngle = 0;
            sample = ParticleRandom_ReadRelativeSample(FanCannon_SampleOrigin(),
                                                       (const u16 *)RANDOM_TABLE_ADDR);
            value = (u16)pos->z - 128;
            sample = (u8)sample;
            pos->z = sample + value;
            pos->y += 128;
            if (!p->reverse)
                angle = 128;
            else
                angle = 0;
        } else {
            s16 sample;
            s32 value;
            baseAngle = 64;
            sample = ParticleRandom_ReadRelativeSample(FanCannon_SampleOrigin(),
                                                       (const u16 *)RANDOM_TABLE_ADDR);
            value = (u16)pos->x - 128;
            sample = (u8)sample;
            pos->x = sample + value;
            pos->y += 128;
            if (!p->reverse)
                angle = 192;
            else
                angle = 64;
        }
        FanCannon_Polar(&velocity, angle, p->wind);
        velocity.y = 0;
        source = (const u16 *)RANDOM_TABLE_ADDR;
        frame = ParticleRandom_ReadRelativeSample(FanCannon_SampleOrigin(), source) & 3;
        if (frame == 3)
            frame = 2;
        frame += 17;
        if (!p->reverse) {
            half = p->range >> 1;
            span = p->range - half;
            distance = half;
            if (span)
                distance += Mod(ParticleRandom_ReadDirectSample(source), span);
            FanCannon_Polar(&offset, baseAngle, distance);
        } else {
            s32 radius = FanCannon_RandomRange(p->range >> 1, source);
            struct SceneVec *out = &offset;
            distance = radius;
            FanCannon_Polar(out, baseAngle, distance);
        }
        source = &offset;
        FanCannon_AddPosition(&position, &position, source);
        EnemyParticle_InitParameters(&parameters);
        EnemyParticle_SetGraphicsParameters(&parameters, 0x7e7c, frame, 0x1bf);
        EnemyParticle_SetMotionParameters(&parameters, 0, 30, angle, p->maximumWind);
        source = gRandomTable;
        if (ParticleRandom_ReadDirectSample(source) & 1)
            EnemyParticle_AddFlags(&parameters, 4);
        if (ParticleRandom_ReadDirectSample(source) & 1)
            EnemyParticle_AddFlags(&parameters, 8);
        EnemyParticle_Emit(&parameters, &position);
    }
    p->wind++;
    if (p->wind > p->maximumWind)
        p->wind = p->maximumWind;
}
