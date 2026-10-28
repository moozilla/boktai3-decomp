#include "directional_force.h"
// MATCHING_CONCESSION: register-pinning
s32 DirectionalForce_Tick(struct DirectionalForce *force)
{
    u32 countdown;
    const u16 *delaySample;
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
        } else
            next = current + (difference >> 4);
        next &= mask;
        force->angle = next;
    }
    asm("" ::: "memory");
    gPlayerBackgroundState = force->magnitude >> 4;
    gPlayerBackgroundMode = force->angle >> 4;
    switch ((s32)force->mode) {
        case 0:
        default:
            goto done;
        case 1:
            countdown = force->delay;
            if (countdown)
                goto decrement;
            if (!force->magnitudeRange)
                force->targetMagnitude = force->baseMagnitude << 4;
            else {
                const u16 *random;
                s32 value;
                gRandomIndex = (gRandomIndex + 1) & 1023;
                random = (const u16 *)RANDOM_TABLE_ADDR;
                value = Mod(random[gRandomIndex], force->magnitudeRange);
                force->targetMagnitude = (force->baseMagnitude + value) << 4;
            }
            {
                register const u16 *random asm("r3") = (const u16 *)RANDOM_TABLE_ADDR;
                u32 *indexAddress = &gRandomIndex;
                u32 oldIndex = *indexAddress;
                u32 index;
                u32 mask;
                index = oldIndex + 1;
                mask = 1023;
                index &= mask;
                *indexAddress = index;
                force->targetAngle = ((const u8 *)random)[index * 2];
                if (!force->delayRange) {
                    countdown = force->baseDelay;
                    goto storeDelay;
                }
                countdown = (index + 1) & mask;
                *indexAddress = countdown;
                delaySample = (const u16 *)(countdown * 2 + (u32)random);
                goto randomDelay;
            }
        case 2:
            countdown = force->delay;
            if (countdown)
                goto decrement;
            if (!force->magnitudeRange)
                force->targetMagnitude = force->baseMagnitude << 4;
            else {
                s32 value;
                gRandomIndex = (gRandomIndex + 1) & 1023;
                value = Mod(((const u16 *)RANDOM_TABLE_ADDR)[gRandomIndex], force->magnitudeRange);
                force->targetMagnitude = (force->baseMagnitude + value) << 4;
            }
            if (!force->delayRange) {
                countdown = force->baseDelay;
                asm("" : "+r"(countdown));
                goto storeDelay;
            }
            {
                u32 *indexAddress = &gRandomIndex;
                u32 index = *indexAddress + 1;
                register u32 mask asm("r1");
                mask = 1023;
                index &= mask;
                *indexAddress = index;
                delaySample = (const u16 *)RANDOM_TABLE_ADDR + index;
            }
        randomDelay: {
            s32 value = Mod(*delaySample, force->delayRange);
            force->delay = force->baseDelay + value;
        }
            goto done;
    }
decrement:
    countdown--;
storeDelay:
    force->delay = countdown;
done:
    return 0;
}
