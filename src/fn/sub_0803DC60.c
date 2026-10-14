#include "scene_pool.h"
#define ScenePool_UpdateSteeringPulse sub_0803DC60
struct ScenePoolSteeringBounds {
    s32 emergencyHigh, high, low, activeHigh, activeLow;
};
static inline u8 AtLeast(s32 value, s32 lo)
{
    u8 result = 1;
    if (value < lo)
        result = 0;
    return result;
}
#ifdef NONMATCHING
// fuzzy: 83.6%
// Remaining differences: pointer lifetimes, materialized bounds checks and narrow RNG loads.
void ScenePool_UpdateSteeringPulse(struct ScenePoolActor *actor, struct ScenePoolEntry *entry,
                                   const struct ScenePoolSteeringBounds *bounds)
{
    u8 active = entry->steeringActive;
    s32 metric;
    if (active) {
        metric = entry->direction;
        if (metric > bounds->activeHigh || !AtLeast(metric, bounds->activeLow)) {
            entry->steeringActive = 0;
            entry->steeringDelay = 30;
            entry->steeringPulse = 0;
            goto done;
        }
        if (!entry->steeringDelay) {
            if (metric > bounds->high)
                entry->steeringPulse = 16;
            else if (metric < bounds->low)
                entry->steeringPulse = -16;
            gUnk_03005308 = (gUnk_03005308 + 1) & 0x3ff;
            entry->steeringDelay = (gUnk_0203B400[gUnk_03005308] & 31) + 60;
        } else
            entry->steeringDelay--;
        {
            u8 *pulseAddress = (u8 *)&entry->steeringPulse;
            u8 pulseByte = *pulseAddress;
            s32 pulse = entry->steeringPulse;
            if (pulse > 0) {
                *pulseAddress = pulseByte - 1;
                actor->controlFlags |= 0x10;
            } else if (pulse < 0) {
                *pulseAddress = pulseByte + 1;
                actor->controlFlags |= 0x20;
            }
        }
    } else {
        metric = entry->direction;
        if (metric <= bounds->high && AtLeast(metric, bounds->low)) {
            entry->steeringActive = 1;
            entry->steeringDelay = 30;
            entry->steeringPulse = active;
            goto done;
        }
        if (metric - 1 > bounds->emergencyHigh)
            actor->controlFlags |= 0x300;
        else if (metric < bounds->low)
            actor->controlFlags |= 0x20;
        else
            actor->controlFlags |= 0x110;
    }
done:;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_0803DC60);
#endif
