#include "directional_force.h"
// MATCHING_CONCESSION: register-pinning
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
static inline s32 readRanges(struct DirectionalForce *force)
{
    u32 magnitude;
    u32 delay;
    register s32 status asm("r0");
    if (Script_SeekToKeyword('p')) {
        force->baseMagnitude = Script_GetValue();
        magnitude = Script_GetValue();
    } else {
        magnitude = 3;
        force->baseMagnitude = magnitude;
    }
    force->magnitudeRange = magnitude;
    delay = Script_SeekToKeyword('t');
    status = delay;
    if (delay) {
        force->baseDelay = Script_GetValue();
        delay = Script_GetValue();
        status = delay;
    } else {
        delay = 120;
        force->baseDelay = delay;
    }
    force->delayRange = delay;
    return status;
}
s32 DirectionalForce_InitializeFromScript(struct DirectionalForce *force, u32 id)
{
    u32 value;
    register s32 status asm("r0");
    force->id = id;
    {
        u32 mode;
        if (Script_SeekToKeyword('m'))
            mode = Script_GetValue();
        else
            mode = 1;
        force->mode = mode;
    }
    {
        u32 found;
        status = Script_SeekToKeyword('H');
        found = status;
        if (found) {
            u16 *destination = &gPlayerDirectionLatch;
            status = Script_GetValue();
            {
                u32 value = status;
                asm("" : : "r"(value));
                *destination = value;
            }
        } else {
            u16 *destination = &gPlayerDirectionLatch;
            *destination = found;
        }
    }
    switch ((s32)force->mode) {
        case 0:
            force->targetAngle = 0;
            force->targetMagnitude = 0;
            break;
        case 1:
            status = readRanges(force);
            {
                register const u16 *random asm("r4") = (const u16 *)RANDOM_TABLE_ADDR;
                u32 *indexAddress = &gRandomIndex;
                u32 oldIndex = *indexAddress;
                u32 index;
                u32 mask;
                index = oldIndex + 1;
                mask = 1023;
                index &= mask;
                *indexAddress = index;
                force->targetAngle = ((const u8 *)random)[index * 2];
                if (!force->magnitudeRange)
                    value = force->baseMagnitude;
                else {
                    u32 next = (index + 1) & mask;
                    s32 sample;
                    *indexAddress = next;
                    status = Mod(random[next], force->magnitudeRange);
                    sample = status;
                    asm("" : : "r"(sample));
                    value = force->baseMagnitude + sample;
                }
            }
            force->targetMagnitude = value << 4;
            force->magnitude = force->baseMagnitude << 4;
            if (!force->delayRange) {
                value = force->baseDelay;
                asm("" : "+r"(value));
                goto storeDelay;
            }
            goto randomDelay;
        case 2: {
            u32 direction = Script_SeekToKeyword('d');
            if (direction)
                direction = Script_GetValue();
            force->targetAngle = direction;
        }
            status = readRanges(force);
            if (!force->magnitudeRange)
                value = force->baseMagnitude;
            else {
                s32 random;
                gRandomIndex = (gRandomIndex + 1) & 1023;
                status = Mod(((u16 *)RANDOM_TABLE_ADDR)[gRandomIndex], force->magnitudeRange);
                random = status;
                asm("" : : "r"(random));
                value = force->baseMagnitude + random;
            }
            force->targetMagnitude = value << 4;
            if (!force->delayRange)
                value = force->baseDelay;
            else {
                s32 random;
            randomDelay:
                gRandomIndex = (gRandomIndex + 1) & 1023;
                status = Mod(((u16 *)RANDOM_TABLE_ADDR)[gRandomIndex], force->delayRange);
                random = status;
                asm("" : : "r"(random));
                value = force->baseDelay + random;
            }
        storeDelay:
            force->delay = value;
            break;
    }
    gDirectionalForce = force;
    return status;
}
