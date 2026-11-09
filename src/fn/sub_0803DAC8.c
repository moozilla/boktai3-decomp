#include "scene_pool.h"

void ScenePool_SetSpeedControlSchedule(struct ScenePoolEntry *, s32, s32, s32);
static inline u8 PlayerHasFlag(u32 mask)
{
    if (gParticipantActors[0] && (gParticipantActors[0]->flags & mask))
        return 1;
    return 0;
}
static inline s32 Within(s32 value, s32 limit)
{
    if (value > limit)
        return 0;
    {
        s32 inside = 1;
        if (value < -limit)
            inside = 0;
        return inside;
    }
}
void ScenePool_ScheduleSpeedControl(struct ScenePoolEntry *entry)
{
    if (PlayerHasFlag(1)) {
        ScenePool_SetSpeedControlSchedule(entry, 2, 4, 8);
        return;
    }
    if (Within(entry->direction, 0x1e000)) {
        if (Within(entry->direction, 0xf000)) {
            s32 random;
            gRandomIndex = (gRandomIndex + 1) & 0x3ff;
            random = *(const u8 *)(RANDOM_TABLE_ADDR + (gRandomIndex << 1));
            if (random <= 31)
                ScenePool_SetSpeedControlSchedule(entry, 2, 2, 4);
            else if (random <= 63)
                ScenePool_SetSpeedControlSchedule(entry, 1, 2, 4);
        } else if (entry->direction > 0)
            ScenePool_SetSpeedControlSchedule(entry, 2, 4, 8);
        else
            ScenePool_SetSpeedControlSchedule(entry, 1, 4, 8);
    } else if (entry->direction > 0) {
        if (entry->direction > 0x50000)
            ScenePool_SetSpeedControlSchedule(entry, 4, 4, 8);
        else if (entry->direction > 0x32000)
            ScenePool_SetSpeedControlSchedule(entry, 3, 4, 8);
        else
            ScenePool_SetSpeedControlSchedule(entry, 2, 4, 8);
    } else
        ScenePool_SetSpeedControlSchedule(entry, 1, 4, 8);
}
