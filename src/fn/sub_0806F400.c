#include "scene_effect_schedule.h"
static inline u8 Occupied(u32 *mask, s32 index)
{
    if (*mask & (1 << index))
        return TRUE;
    return FALSE;
}
struct SceneEffectScheduleEntry *SceneEffectSchedule_Allocate(struct SceneEffectSchedule *manager)
{
    s32 i;
    for (i = 0; i < 12; i++) {
        if (!Occupied(&manager->occupiedMask, i)) {
            manager->occupiedMask |= 1 << i;
            manager->entries[i].index = i;
            return &manager->entries[i];
        }
    }
    return 0;
}
