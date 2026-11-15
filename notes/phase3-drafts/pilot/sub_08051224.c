#include "directional_force.h"

static inline u32 Random_Next(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + (u32)gRandomTable);
}

static inline u32 Random_NextFrom(const u16 *table)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + (u32)table);
}

static inline u32 Random_NextByteFrom(const u16 *table)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u8 *)(offset + (u32)table);
}

s32 DirectionalForce_Tick(struct DirectionalForce *force)
{
    const u16 *table;

    force->magnitude = (force->magnitude * 15 + force->targetMagnitude) >> 4;
    {
        s32 target = force->targetAngle << 4;
        s32 current = force->angle;
        s32 difference = current - 4096;
        u32 mask;
        s32 next;
        difference = target - difference;
        mask = 4095;
        difference &= mask;
        if (difference > 2048) {
            s32 full = 4096;
            difference = full - difference;
            next = current + full;
            difference >>= 4;
            next -= difference;
            force->angle = next & mask;
        } else {
            next = current + (difference >> 4);
            force->angle = next & mask;
        }
    }
    gPlayerBackgroundState = force->magnitude >> 4;
    gPlayerBackgroundMode = force->angle >> 4;
    switch (force->mode) {
        case 0:
        default:
            break;
        case 1:
            if (force->delay == 0) {
            if (force->magnitudeRange == 0)
                force->targetMagnitude = force->baseMagnitude << 4;
            else
                force->targetMagnitude = (force->baseMagnitude + Mod(Random_Next(), force->magnitudeRange)) << 4;
            table = gRandomTable;
            force->targetAngle = Random_NextByteFrom(table);
            if (force->delayRange == 0)
                force->delay = force->baseDelay;
            else
                force->delay = force->baseDelay + Mod(Random_NextFrom(table), force->delayRange);
            } else
                force->delay--;
            break;
        case 2:
            if (force->delay == 0) {
            if (force->magnitudeRange == 0)
                force->targetMagnitude = force->baseMagnitude << 4;
            else
                force->targetMagnitude = (force->baseMagnitude + Mod(Random_Next(), force->magnitudeRange)) << 4;
            if (force->delayRange == 0)
                force->delay = force->baseDelay;
            else
                force->delay = force->baseDelay + Mod(Random_Next(), force->delayRange);
            } else
                force->delay--;
            break;
    }
    return 0;
}
