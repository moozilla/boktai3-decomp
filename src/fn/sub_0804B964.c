#ifdef NONMATCHING
// fuzzy: 97.9%
/* Readable distance limiter; only four scratch-register differences remain. */
#include "tether_actor.h"
// MATCHING_CONCESSION: register-pinning
void TetherActor_BlockPlayerAcrossTerrain(struct TetherActor *);
static inline s32 truncateShift(s32 value, u32 shift)
{
    if (value >= 0)
        return value >> shift;
    return -(-value >> shift);
}
static inline s32 cosine(s32 angle, s32 radius)
{
    s32 sample = gUnk_086149C4[(angle + 64) & 255];
    register s32 product asm("r0");
    product = radius;
    product *= sample;
    {
        s32 result = truncateShift(product, 12);
        asm("" : "+r"(result));
        return result;
    }
}
static inline s32 sine(s32 angle, s32 radius)
{
    register s32 sample asm("r1") = gUnk_086149C4[angle & 255];
    s32 product;
    product = radius;
    product *= sample;
    return truncateShift(product, 12);
}
void TetherActor_LimitPlayerDistance(struct TetherActor *input)
{
    struct TetherActor *actor = input;
    struct TetherPlayer **savedSlot;
    s32 angle;
    register s32 offsetX asm("r10");
    s32 offsetZ;
    register s16 *savedX asm("r8");
    register s16 *savedZ asm("r9");
    s16 *sourceX;
    s16 *sourceZ;
    s32 dx;
    s32 dz;
    {
        register struct TetherPlayer **slot asm("r3") = gUnk_02000580;
        register u32 value asm("r0") = (*slot)->facing;
        const s16 *table;
        s32 sample;
        s32 product;
        value = (value + 1) & 7;
        angle = value << 5;
        table = gUnk_086149C4;
        sample = table[(angle + 64) & 255];
        product = 70;
        product *= sample;
        savedSlot = slot;
        offsetX = truncateShift(product, 12);
        sample = table[angle & 255];
        product = 70;
        product *= sample;
        product = truncateShift(product, 12);
        offsetZ = product;
    }
    {
        struct TetherPlayer *player = *savedSlot;
        s32 value = player->position.x + offsetX;
        sourceX = &actor->position.x;
        {
            register s32 other asm("r1") = *sourceX;
            dx = value - other;
        }
        value = player->position.z + offsetZ;
        sourceZ = &actor->position.z;
        {
            register s32 other asm("r1") = *sourceZ;
            dz = value - other;
        }
    }
    {
        s32 squaredX = dx * dx;
        s32 squaredZ = dz * dz;
        s32 limit;
        squaredX += squaredZ;
        limit = 22500;
        savedX = sourceX;
        savedZ = sourceZ;
        if (squaredX <= limit)
            return;
    }
    angle = Math_Atan2(dx, dz);
    {
        struct TetherPlayer **slot;
        s32 radialX = cosine(angle, 150);
        s32 radialZ = sine(angle, 150);
        s32 targetX;
        register s32 targetZ asm("r4");
        register s16 *axis asm("r3") = savedX;
        s32 sample = *axis;
        s32 value = offsetX;
        sample -= value;
        targetX = sample + radialX;
        axis = savedZ;
        value = *axis;
        value -= offsetZ;
        targetZ = value + radialZ;
        slot = gUnk_02000580;
        value = (*slot)->position.x * 3 + targetX;
        savedSlot = slot;
        {
            s32 result = truncateShift(value, 2);
            (*savedSlot)->position.x = result;
        }
        value = (*savedSlot)->position.z * 3 + targetZ;
        {
            register s32 result asm("r1") = truncateShift(value, 2);
            (*savedSlot)->position.z = result;
        }
        TetherActor_BlockPlayerAcrossTerrain(actor);
        {
            u32 *words = (u32 *)*savedSlot;
            register u32 offset asm("r3") = 0x168;
            register u32 *destination asm("r2");
            u32 second;
            u32 first;
            asm("" : "+r"(offset));
            destination = (u32 *)((u8 *)words + offset);
            second = words[0x34 / 4];
            first = words[0x30 / 4];
            destination[0] = first;
            destination[1] = second;
        }
    }
}

#else
#include "global.h"
INCLUDE_ASM("asm/nonmatching", TetherActor_LimitPlayerDistance);
#endif
