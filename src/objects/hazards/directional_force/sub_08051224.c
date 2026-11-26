#include "objects/hazards/directional_force.h"
#include "engine/math/monster_particle_random.h"

/* Smoothed angles use four fractional bits over an eight-bit turn. */
enum {
    DIRECTIONAL_FORCE_FULL_TURN = 4096,
    DIRECTIONAL_FORCE_HALF_TURN = DIRECTIONAL_FORCE_FULL_TURN / 2,
    DIRECTIONAL_FORCE_ANGLE_MASK = DIRECTIONAL_FORCE_FULL_TURN - 1,
};

static inline u32 Random_Next(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + (u32)gRandomTable);
}

static inline u32 Random_NextByteFrom(const u16 *table)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u8 *)(offset + (u32)table);
}

/* Draw a sample, leaving the table it was read from in *table. */
static inline u32 Random_NextKeepTable(const u16 **table)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    *table = gRandomTable;
    return *(const u16 *)(offset + (u32)*table);
}

s32 DirectionalForce_Tick(struct DirectionalForce *force)
{
    const u16 *table;

    force->magnitude = (force->magnitude * 15 + force->targetMagnitude) >> 4;
    {
        s32 target = force->targetAngle << 4;
        s32 current = force->angle;
        s32 difference = current - DIRECTIONAL_FORCE_FULL_TURN;
        s32 next;
        difference = target - difference;
        difference &= DIRECTIONAL_FORCE_ANGLE_MASK;
        if (difference > DIRECTIONAL_FORCE_HALF_TURN) {
            difference = DIRECTIONAL_FORCE_FULL_TURN - difference;
            next = current + DIRECTIONAL_FORCE_FULL_TURN;
            difference >>= 4;
            next -= difference;
            force->angle = next & DIRECTIONAL_FORCE_ANGLE_MASK;
        } else {
            next = current + (difference >> 4);
            force->angle = next & DIRECTIONAL_FORCE_ANGLE_MASK;
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
                    force->targetMagnitude =
                        (force->baseMagnitude + Mod(Random_Next(), force->magnitudeRange)) << 4;
                table = gRandomTable;
                force->targetAngle = Random_NextByteFrom(table);
                if (force->delayRange == 0)
                    force->delay = force->baseDelay;
                else
                    force->delay =
                        force->baseDelay + Mod(ParticleRandom_ReadSample(table), force->delayRange);
            } else
                force->delay--;
            break;
        case 2:
            if (force->delay == 0) {
                if (force->magnitudeRange == 0)
                    force->targetMagnitude = force->baseMagnitude << 4;
                else
                    force->targetMagnitude =
                        (force->baseMagnitude +
                         Mod(Random_NextKeepTable(&table), force->magnitudeRange))
                        << 4;
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
