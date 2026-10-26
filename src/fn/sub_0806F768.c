#include "scene_effect_schedule.h"
struct SceneEffectScheduleEntry *SceneEffectSchedule_FindById(u32);
void SceneEffectSchedule_RemoveFromScript(void)
{
    struct SceneEffectSchedule *manager = gSceneEffectSchedule;
    s32 id;
    struct SceneEffectScheduleEntry *entry;
    if (!manager)
        return;
    id = Script_GetKeywordValue('n', -1);
    if (id < 0)
        return;
    entry = SceneEffectSchedule_FindById(id);
    if (!entry)
        return;
    {
        s32 index = entry->index;
        u32 *mask;
        mask = &manager->occupiedMask;
        *mask &= ~(1u << index);
    }

    Memory_Clear(entry, sizeof(*entry));
}
