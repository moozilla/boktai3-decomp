#include "objects/hazards/directional_force.h"
#include "engine/math/monster_particle_random.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);

static inline u16 NextRandom(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return *(const u16 *)((gRandomIndex << 1) + RANDOM_TABLE_ADDR);
}

static inline void ReadRanges(struct DirectionalForce *force)
{
    u32 value;
    {
        u32 magnitude;
        if (Script_SeekToKeyword('p')) {
            force->baseMagnitude = Script_GetValue();
            magnitude = Script_GetValue();
        } else {
            magnitude = 3;
            force->baseMagnitude = magnitude;
        }
        force->magnitudeRange = magnitude;
    }
    value = Script_SeekToKeyword('t');
    if (value) {
        force->baseDelay = Script_GetValue();
        value = Script_GetValue();
    } else {
        value = 120;
        force->baseDelay = value;
    }
    force->delayRange = value;
}

// No return statement: the caller tests whatever the last call left in r0, as in the ROM.
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
            gPlayerDirectionLatch = Script_GetValue();
        else
            gPlayerDirectionLatch = found;
    }
    switch (force->mode) {
        case 0:
            force->targetAngle = 0;
            force->targetMagnitude = 0;
            break;
        case 1:
            ReadRanges(force);
            {
                const u16 *random = gRandomTable;
                u32 index = (gRandomIndex + 1) & 1023;
                gRandomIndex = index;
                force->targetAngle = ((const u8 *)random)[index * 2];
                if (!force->magnitudeRange)
                    value = force->baseMagnitude;
                else {
                    u32 next = (index + 1) & 1023;
                    u32 sample;
                    gRandomIndex = next;
                    /* The shared macro's loop notes count random's refs twice in flow,
                       so global alloc gives it r4 before the 1023 mask. */
                    PARTICLE_RANDOM_READ_SAMPLE(sample, random, next);
                    value = force->baseMagnitude + Mod(sample, force->magnitudeRange);
                }
            }
            force->targetMagnitude = value << 4;
            force->magnitude = force->baseMagnitude << 4;
            /* Stored directly (a HImode copy), so jump2 does not cross-jump this arm into
               case 2's; the random arm is shared with case 2's. */
            if (!force->delayRange)
                force->delay = force->baseDelay;
            else {
                value = force->baseDelay + Mod(NextRandom(), force->delayRange);
                force->delay = value;
            }
            break;
        case 2:
            {
                u32 direction = Script_SeekToKeyword('d');
                if (direction)
                    direction = Script_GetValue();
                force->targetAngle = direction;
            }
            ReadRanges(force);
            if (!force->magnitudeRange)
                value = force->baseMagnitude;
            else
                value = force->baseMagnitude + Mod(NextRandom(), force->magnitudeRange);
            force->targetMagnitude = value << 4;
            if (!force->delayRange)
                value = force->baseDelay;
            else
                value = force->baseDelay + Mod(NextRandom(), force->delayRange);
            force->delay = value;
            break;
    }
    gDirectionalForce = force;
#ifdef UBFIX
    // falls off the end with r0 left over from its last statement; the creator only tests
    // < 0, and this initializer has no failure path here
    return 0;
#endif
}
