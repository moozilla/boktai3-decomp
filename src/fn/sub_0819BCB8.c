// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "engine.h"
#include "monster_particle_random.h"
#include "ram.h"

/* Tentative: emit a motion entry from a randomized point ahead or behind the actor. */

struct BurstVector {
    s16 x, y, z, unk_06;
};
struct ActorReaction {
    u8 unk_00[0x54];
    struct BurstVector position;
    struct BurstVector velocity;
    u8 unk64[0xA8 - 0x64];
    u8 motionFacing;
    u8 unkA9[0x110 - 0xA9];
    struct BurstVector *parts[8];
    u8 unk130[0x32B - 0x130];
    u8 side;
};
extern const s16 gUnk_086149C4[];
void MotionEntry_CreateWithDelta(struct BurstVector *, struct BurstVector *, s32, s32);
static inline s32 Scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
static inline void Polar(struct BurstVector *out, s32 inputAngle, s32 radius)
{
    register s32 angle asm("r3") = inputAngle;
    out->x = Scale(gUnk_086149C4[(angle + 64) & 255] * radius);
    out->y = 0;
    out->z = Scale(gUnk_086149C4[angle & 255] * radius);
}
static inline void Add(struct BurstVector *out, struct BurstVector *a, struct BurstVector *b)
{
    out->x = b->x + a->x;
    out->y = b->y + a->y;
    out->z = b->z + a->z;
}
static inline s32 RandomValue(void)
{
    u32 table = (u32)gRandomTable;
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + table);
}
u16 Sqrt(u32);
s32 Div(s32, s32);
static inline void MotionPolar(struct BurstVector *out, s32 angle, s32 radius)
{
    out->x = Scale(gUnk_086149C4[(angle + 64) & 255] * radius);
    out->y = 0;
    out->z = Scale(gUnk_086149C4[angle & 255] * radius);
}
static inline void Difference(struct BurstVector *out, struct BurstVector *a, struct BurstVector *b)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}
static inline void Normalize(struct BurstVector *v, s32 speed)
{
    struct BurstVector *out = v;
    u16 length = Sqrt(v->x * v->x + out->y * out->y + out->z * out->z);
    if (length) {
        v->x = Div(v->x * speed, length);
        out->y = Div(out->y * speed, length);
        out->z = Div(out->z * speed, length);
    }
}

void Hresvelgr_EmitFacingMotion(struct ActorReaction *input)
{
    struct ActorReaction *actor = input;
    struct ActorReaction *saved = actor;
    struct BurstVector position;
    struct BurstVector velocity;
    /* The inlined RNG retains this setup address across its first two samples. */
    u32 origin = PARTICLE_RANDOM_SAMPLE_ORIGIN;
    u32 savedOrigin;
    u32 sideSample;
    s32 side;
    s32 facing;
    s32 radius;
    s32 direction;
    sideSample = RandomValue() >> 3;
    side = sideSample - sideSample / 2 * 2;
    facing = actor->side;
    radius = RandomValue() >> 3;
    {
        register s32 quotient asm("r1");

        struct BurstVector *out;
        s32 angle;
        quotient = radius;
        asm("" : : "r"(quotient));
        savedOrigin = origin;
        quotient = quotient / 1024 * 1024;
        out = &position;
        side <<= 7;
        direction = facing << 6;
        angle = side + direction;
        angle += 64;
        radius -= quotient;
        out->x = Scale(gUnk_086149C4[(angle + 64) & 255] * radius);
        out->y = 0;
        angle &= 255;
        direction = angle << 1;
        out->z = Scale(*(const s16 *)((u32)gUnk_086149C4 + direction) * radius);
    }
    Add(&position, &saved->position, &position);
    direction = actor->side;
    Polar(&velocity, direction << 6, 128);

    MotionEntry_CreateWithDelta(
        &position, &velocity,
        ((s32)ParticleRandom_ReadRelativeSample(savedOrigin, (const u16 *)RANDOM_TABLE_ADDR) >> 3) %
            4,
        32);
}
#define Hresvelgr_SetPartApproachMotion sub_0819BE28
/* Tentative: choose an offset from one part, then set a normalized movement vector. */
void Hresvelgr_SetPartApproachMotion(struct ActorReaction *actor, s32 radius, s32 speed, u32 part)
{
    struct BurstVector offset;
    MotionPolar(&offset, ((actor->motionFacing >> 1) << 6) + 128, radius);
    {
        struct BurstVector *target = actor->parts[part];
        Add(&offset, target, &offset);
    }
    Difference(&actor->velocity, &offset, &actor->position);
    Normalize(&actor->velocity, speed);
}
