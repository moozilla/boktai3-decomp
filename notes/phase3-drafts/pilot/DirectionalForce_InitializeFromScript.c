#include "directional_force.h"
#include "monster_particle_random.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
static inline void readRanges(struct DirectionalForce *force)
{
    u32 magnitude, delay;
    if (Script_SeekToKeyword('p')) {
        force->baseMagnitude = Script_GetValue();
        magnitude = Script_GetValue();
    } else {
        magnitude = 3;
        force->baseMagnitude = magnitude;
    }
    force->magnitudeRange = magnitude;
    if (Script_SeekToKeyword('t')) {
        force->baseDelay = Script_GetValue();
        delay = Script_GetValue();
    } else {
        delay = 120;
        force->baseDelay = delay;
    }
    force->delayRange = delay;
}
static inline u32 randomSample(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return ((u16 *)RANDOM_TABLE_ADDR)[gRandomIndex];
}
static inline u32 randomSampleFrom(const u16 *table)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return table[gRandomIndex];
}
s32 DirectionalForce_InitializeFromScript(struct DirectionalForce *force, u32 id)
{
    u32 value;
    force->id = id;
    if (Script_SeekToKeyword('m'))
        force->mode = Script_GetValue();
    else
        force->mode = 1;
    if (Script_SeekToKeyword('H'))
        gPlayerDirectionLatch = Script_GetValue();
    else
        gPlayerDirectionLatch = 0;
    switch (force->mode) {
        case 0:
            force->targetAngle = 0;
            force->targetMagnitude = 0;
            break;
        case 1: {
            const u16 *table;
            u32 *seed;
            u32 mask;
            readRanges(force);
            table = gRandomTable;
            seed = &gRandomIndex;
            {
                u32 index = *seed + 1;
                mask = 0x3FF;
                index &= mask;
                *seed = index;
                force->targetAngle = (u8)table[*seed];
            }
            if (!force->magnitudeRange)
                value = force->baseMagnitude;
            else
                value = force->baseMagnitude + Mod(ParticleRandom_NextSample(table, seed, mask), force->magnitudeRange);
        }
            force->targetMagnitude = value << 4;
            force->magnitude = force->baseMagnitude << 4;
            if (!force->delayRange) {
                force->delay = force->baseDelay;
                break;
            }
            goto randomDelay;
        case 2:
            if (Script_SeekToKeyword('d'))
                force->targetAngle = Script_GetValue();
            else
                force->targetAngle = 0;
            readRanges(force);
            if (!force->magnitudeRange)
                value = force->baseMagnitude;
            else
                value = force->baseMagnitude + Mod(randomSample(), force->magnitudeRange);
            force->targetMagnitude = value << 4;
            if (!force->delayRange)
                value = force->baseDelay;
            else
            randomDelay:
                value = force->baseDelay + Mod(randomSample(), force->delayRange);
            force->delay = value;
            break;
    }
    gDirectionalForce = force;
}
