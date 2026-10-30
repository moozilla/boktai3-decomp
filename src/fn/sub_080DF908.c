#include "contact_actor.h"
#include "engine.h"
#include "ram.h"
#include "heap.h"

extern const s16 gUnk_086149C4[];
u32 SceneMovingParticle_CreateWithOptions(void *, void *, s32, s32, s32);

static inline u32 DogTrail_NextSample(const u16 *table)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return table[gRandomIndex];
}

static inline u32 DogTrail_ReadSampleAddress(u32 address, u32 origin, const u16 *table)
{
    u32 offset = address - origin;
    return *(const u16 *)((const u8 *)table + offset);
}

static inline u32 DogTrail_NextBankSample(u32 origin, const u16 *table)
{
    u32 address;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    address = origin + gRandomIndex * sizeof(*table);
    return DogTrail_ReadSampleAddress(address, origin, table);
}

static inline s32 DogTrail_Truncate(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}

void SceneActorAnimation_SpawnTrailAtPosition(void *position, u32 palette)
{
    s16 velocity[3];
    s16 *planar;
    u32 randomOrigin;
    const u16 *earlyTable;
    u32 mask, sampleOrigin;
    s32 heading;
    s32 speed, vertical;
    s16 *result;

    /* The recovered bank origin cancels when converting a sample address to its table offset. */
    randomOrigin = (u32)&gHeapHead + 0x0203B000u;
    earlyTable = (const u16 *)RANDOM_TABLE_ADDR;
    mask = 255;
    heading = DogTrail_NextSample(earlyTable) & mask;
    {
        s32 raw = DogTrail_NextSample(earlyTable);
        s32 speedMask = 31;
        planar = velocity;
        raw &= speedMask;
        speed = raw;
    }
    {
        s32 x, zero, rawX;
        rawX = gUnk_086149C4[(heading + 64) & mask] * speed;
        sampleOrigin = randomOrigin;
        x = DogTrail_Truncate(rawX);
        zero = 0;
        planar[0] = x;
        planar[1] = zero;
        planar[2] = DogTrail_Truncate(gUnk_086149C4[heading] * speed);
    }
    vertical = DogTrail_NextBankSample(sampleOrigin, (const u16 *)RANDOM_TABLE_ADDR);
    vertical &= 63;
    vertical -= 48;
    result = velocity;
    result[1] = vertical;
    if (palette == 3)
        SceneMovingParticle_CreateWithOptions(position, result, 60, 3, 3);
    else
        SceneMovingParticle_CreateWithOptions(position, result, 60, 4, 3);
}
