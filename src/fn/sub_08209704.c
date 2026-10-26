#include "global.h"
#include "engine.h"
/* Coordinates wrap at 16 bits when particle offsets are added. */
struct FanVector {
    u16 x, y, z, pad;
};

struct FanCannon {
    s32 age;
    u8 unk_04[0x28];
    struct FanVector position;
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
extern const u16 gUnk_0203B400[];
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
static inline const u16 *RandomAddress(void)
{
    gRandomIndex = (gRandomIndex + 1) & 0x3ff;
    return (const u16 *)RANDOM_TABLE_ADDR + gRandomIndex;
}
static inline s32 RandomRange(s32 range)
{
    s32 result = 0;
    if (range)
        result = Mod(RandomFrom(gUnk_0203B400), range);
    return result;
}
static inline s32 Scale(s32 value)
{
    return value >= 0 ? value >> 12 : -((-value) >> 12);
}
static inline void Polar(struct FanVector *out, s32 angle, s32 radius)
{
    out->x = Scale(gUnk_086149C4[(angle + 64) & 255] * radius);
    out->y = 0;
    out->z = Scale(gUnk_086149C4[angle] * radius);
}
static inline void Add(struct FanVector *out, struct FanVector *a, struct FanVector *b)
{
    out->x = b->x + a->x;
    out->y = a->y + b->y;
    out->z = b->z + a->z;
}
#ifdef NONMATCHING
// fuzzy: 67.0%
// scope shape 58 (differences once low registers are set aside)
u32 FanCannon_UpdateActive(struct FanCannon *p)
{
    struct ParticleParameters parameters;
    struct FanVector position, velocity, offset;
    struct FanVector *pos;
    s16 baseAngle;
    u8 angle;
    s32 frame, distance, half, span;
    s32 direction;
    if (Mod(p->age, 30) == 0) {
        position = p->position;
        direction = p->direction;
        pos = &position;
        if (direction == 0) {
            const u16 *sample;
            baseAngle = 0;
            sample = RandomAddress();
            {
                s32 value = pos->z - 128;
                pos->z = value + (*(const u8 *)sample);
            }
            pos->y += 128;
            if (!p->reverse)
                angle = 128;
            else
                angle = 0;
        } else {
            const u16 *sample;
            baseAngle = 64;
            sample = RandomAddress();
            {
                s32 value = pos->x - 128;
                pos->x = value + (*(const u8 *)sample);
            }
            pos->y += 128;
            if (!p->reverse)
                angle = 192;
            else
                angle = 64;
        }
        /* Compute the wind vector alongside the particle position. */
        Polar(&velocity, angle, p->wind);
        velocity.y = 0;
        frame = RandomFrom(gUnk_0203B400) & 3;
        if (frame == 3)
            frame = 2;
        frame += 17;
        if (!p->reverse) {
            half = p->range >> 1;
            span = p->range - half;
            distance = half;
            if (span)
                distance += Mod(RandomFrom(gUnk_0203B400), span);
            Polar(&offset, baseAngle, distance);
        } else {
            half = p->range >> 1;
            distance = RandomRange(half);
            Polar(&offset, baseAngle, distance);
        }
        Add(pos, pos, &offset);
        EnemyParticle_InitParameters(&parameters);
        EnemyParticle_SetGraphicsParameters(&parameters, 0x7e7c, frame, 0x1bf);
        EnemyParticle_SetMotionParameters(&parameters, 0, 30, angle, p->maximumWind);
        if (RandomFrom(gUnk_0203B400) & 1)
            EnemyParticle_AddFlags(&parameters, 4);
        if (RandomFrom(gUnk_0203B400) & 1)
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
