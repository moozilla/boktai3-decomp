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
void EnemyParticle_Emit(void *, void *);

static inline u32 RandomFrom(const u16 *table)
{
    gRandomIndex = (gRandomIndex + 1) & 0x3ff;
    return table[gRandomIndex];
}
static inline s32 RandomRange(s32 range)
{
    s32 result = 0;
    if (range)
        result = Mod(RandomFrom(gRandomTable), range);
    return result;
}
static inline s32 Scale(s32 value)
{
    return value >= 0 ? value >> 12 : -((-value) >> 12);
}
static inline void FanCannon_Polar(struct SceneVec *out, u8 angle, s32 radius)
{
    s16 cosine, sine;
    cosine = gUnk_086149C4[(angle + 64) & 255];
    out->x = Scale(radius * cosine);
    out->y = 0;
    sine = gUnk_086149C4[angle];
    out->z = Scale(sine * radius);
}
static inline void Add(struct SceneVec *out, struct SceneVec *a, struct SceneVec *b)
{
    out->x = b->x + a->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
}
#ifdef NONMATCHING
// fuzzy: 84.5%
u32 FanCannon_UpdateActive(struct FanCannon *p)
{
    struct ParticleParameters parameters;
    struct SceneVec position, velocity, offset;
    struct SceneVec *pos;
    u8 baseAngle;
    u8 angle;
    s32 frame, distance, half, span;
    s32 direction;
    u32 frameOrigin;
    const u16 *rangeTable;
    u32 origin;
    if (Mod(p->age, 30) == 0) {
        position = p->position;
        direction = p->direction;
        pos = &position;
        if (direction == 0) {
            s16 sample;
            s32 value;
            baseAngle = 0;
            origin = PARTICLE_RANDOM_SAMPLE_ORIGIN;
            sample = ParticleRandom_ReadRelativeSample(origin, gRandomTable);
            value = pos->z - 128;
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
            origin = PARTICLE_RANDOM_SAMPLE_ORIGIN;
            sample = ParticleRandom_ReadRelativeSample(origin, (const u16 *)RANDOM_TABLE_ADDR);
            value = pos->x - 128;
            sample = (u8)sample;
            pos->x = sample + value;
            pos->y += 128;
            if (!p->reverse)
                angle = 192;
            else
                angle = 64;
        }
        /* Compute the wind vector alongside the particle position. */
        frameOrigin = origin;
        FanCannon_Polar(&velocity, angle, p->wind);
        velocity.y = 0;
        rangeTable = gRandomTable;
        frame = ParticleRandom_ReadRelativeSample(frameOrigin, gRandomTable) & 3;
        if (frame == 3)
            frame = 2;
        frame += 17;
        if (!p->reverse) {
            half = p->range >> 1;
            span = p->range - half;
            distance = half;
            if (span)
                distance += Mod(RandomFrom(rangeTable), span);
            FanCannon_Polar(&offset, baseAngle, distance);
        } else {
            half = p->range >> 1;
            distance = RandomRange(half);
            FanCannon_Polar(&offset, baseAngle, distance);
        }
        Add(pos, &offset, pos);
        EnemyParticle_InitParameters(&parameters);
        EnemyParticle_SetGraphicsParameters(&parameters, 0x7e7c, frame, 0x1bf);
        EnemyParticle_SetMotionParameters(&parameters, 0, 30, angle, p->maximumWind);
        if (RandomFrom(gRandomTable) & 1)
            EnemyParticle_AddFlags(&parameters, 4);
        if (RandomFrom(gRandomTable) & 1)
            EnemyParticle_AddFlags(&parameters, 8);
        EnemyParticle_Emit(&parameters, pos);
    }
    p->wind++;
    if (p->wind > p->maximumWind)
        p->wind = p->maximumWind;
}
#else
INCLUDE_ASM("asm/nonmatching", FanCannon_UpdateActive);
#endif
