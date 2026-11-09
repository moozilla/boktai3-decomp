#include "scene_pool.h"
static inline s32 Within(s32 value, s32 high, s32 low)
{
    if (value > high)
        return 0;
    {
        s32 inside = 1;
        if (value < low)
            inside = 0;
        return inside;
    }
}
void ScenePool_UpdateSpeedPulse(struct ScenePoolActor *actor, struct ScenePoolEntry *entry,
                                const struct ScenePoolSpeedBounds *bounds)
{
    u32 active = entry->speedControlActive;
    s32 metric;
    if (active) {
        metric = entry->direction;
        if (!Within(metric, bounds->activeHigh, bounds->activeLow)) {
            entry->speedControlActive = 0;
            entry->speedControlDelay = 30;
            entry->speedControlPulse = 0;
            return;
        }
        if (!entry->speedControlDelay) {
            if (metric > bounds->high)
                entry->speedControlPulse = 16;
            else if (metric < bounds->low)
                entry->speedControlPulse = -16;
            gRandomIndex = (gRandomIndex + 1) & 0x3ff;
            {
                u32 random = *(u16 *)(RANDOM_TABLE_ADDR + (gRandomIndex << 1));
                entry->speedControlDelay = (random & 31) + 60;
            }
        } else
            entry->speedControlDelay--;
        if (entry->speedControlPulse > 0) {
            entry->speedControlPulse--;
            actor->controlFlags |= 0x10;
        } else if (entry->speedControlPulse < 0) {
            entry->speedControlPulse++;
            actor->controlFlags |= 0x20;
        }
    } else {
        s32 high = bounds->high;
        metric = entry->direction;
        if (Within(metric, high, bounds->low)) {
            entry->speedControlActive = 1;
            entry->speedControlDelay = 30;
            entry->speedControlPulse = active;
            return;
        }
        if (metric - 1 > bounds->emergencyHigh)
            actor->controlFlags |= 0x300;
        else if (metric < bounds->low)
            actor->controlFlags |= 0x20;
        else
            actor->controlFlags |= 0x110;
    }
}
