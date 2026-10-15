#include "directional_force.h"
// MATCHING_CONCESSION: register-pinning
#define DirectionalForce_InitializeFromScript sub_080513B8
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
static inline void readRanges(struct DirectionalForce *force)
{
    u32 magnitude;
    u32 delay;
    if (Script_SeekToKeyword('p')) {
        force->baseMagnitude = Script_GetValue();
        magnitude = Script_GetValue();
    } else {
        magnitude = 3;
        force->baseMagnitude = magnitude;
    }
    force->magnitudeRange = magnitude;
    delay = Script_SeekToKeyword('t');
    if (delay) {
        force->baseDelay = Script_GetValue();
        delay = Script_GetValue();
    } else {
        delay = 120;
        force->baseDelay = delay;
    }
    force->delayRange = delay;
}
s32 DirectionalForce_InitializeFromScript(struct DirectionalForce *force, u32 id)
{
    u32 value;
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
        u32 found = Script_SeekToKeyword('H');
        if (found)
            gUnk_0200052C = Script_GetValue();
        else
            gUnk_0200052C = found;
    }
    switch ((s32)force->mode) {
        case 0:
            force->targetAngle = 0;
            force->targetMagnitude = 0;
            break;
        case 1:
            readRanges(force);
            {
                register const u16 *random asm("r4") = (const u16 *)RANDOM_TABLE_ADDR;
                u32 *indexAddress = &gUnk_03005308;
                register u32 oldIndex asm("r1") = *indexAddress;
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
                    register s32 sample asm("r2");
                    *indexAddress = next;
                    sample = Mod(random[next], force->magnitudeRange);
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
            readRanges(force);
            if (!force->magnitudeRange)
                value = force->baseMagnitude;
            else {
                register s32 random asm("r2");
                gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
                random = Mod(((u16 *)RANDOM_TABLE_ADDR)[gUnk_03005308], force->magnitudeRange);
                value = force->baseMagnitude + random;
            }
            force->targetMagnitude = value << 4;
            if (!force->delayRange)
                value = force->baseDelay;
            else {
                register s32 random asm("r2");
            randomDelay:
                gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
                random = Mod(((u16 *)RANDOM_TABLE_ADDR)[gUnk_03005308], force->delayRange);
                value = force->baseDelay + random;
            }
        storeDelay:
            force->delay = value;
            break;
    }
    gUnk_02000114 = force;
}
