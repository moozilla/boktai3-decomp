#include "global.h"
#include "engine.h"
struct SceneStatusIndicatorSlot {
    u8 unknown0[0x2A];
    u8 active, index;
    u16 reset, timer;
};
struct SceneStatusIndicatorManager {
    u8 unknown0[0x1C];
    struct SceneStatusIndicatorSlot slots[12];
    u32 allocated;
};
extern void *gUnk_08610758[];
static inline u8 allocated(u32 *flags, s32 index)
{
    if (*flags & (1 << index))
        return 1;
    return 0;
}
static inline u32 *SlotMask(struct SceneStatusIndicatorManager *manager)
{
    return &manager->allocated;
}
s32 SceneStatusIndicator_UpdateManager(struct SceneStatusIndicatorManager *manager)
{
    struct SceneStatusIndicatorSlot *start = manager->slots;
    struct SceneStatusIndicatorSlot *entry;
    s32 i;
    u32 *flags;
    void **callbacks;
    u8 *active;
    if (!manager->allocated)
        return 0;
    i = 0;
    flags = SlotMask(manager);
    callbacks = gUnk_08610758;
    active = &manager->slots[0].active;
    entry = start;
    do {
        if (allocated(flags, i))
            sub_08249240(manager, entry, callbacks[*active]);
        active += sizeof(*entry);
        entry++;
        i++;
    } while (i <= 11);
    return 0;
}
