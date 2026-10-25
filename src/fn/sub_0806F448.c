#include "scene_effect_schedule.h"
s32 Div(s32, s32);
static inline s32 Scale(s32 input)
{
    s32 value = input;
    u32 flags = gUnk_030053F4;
    u32 mask = 0x800;
    if (flags & mask)
        return value;
    return Div((10 - gUnk_02000710->unk876) * value, 10);
}
s32 SceneEffectSchedule_Tick(struct SceneEffectSchedule *manager)
{
    struct SceneEffectScheduleEntry *initial = manager->entries;
    struct SceneEffectScheduleEntry *entry;
    s32 i;
    if (!manager->occupiedMask)
        return 0;
    {
        u32 mask = 0x400;
        if ((gUnk_030053F4 | gUnk_030053F0) & mask)
            return 0;
    }
    {
        u32 mask = 4;
        if (gUnk_02000560 & mask)
            return 0;
    }
    entry = initial;
    for (i = 11; i >= 0; entry++, i--) {
        if (entry->effect && entry->effect->state < 0) {
            s32 scale = Scale(entry->scaleValue);
            s32 random = Scale(entry->randomValue);
            entry->effect = SceneScheduledEffect_Create(&entry->position, entry->id, scale, entry->kind,
                                         entry->style, -1, entry->offset.x, entry->offset.y,
                                         entry->offset.z, random, entry->flags);
        }
    }
    return 0;
}
