#include "hit_switch.h"
void HitSwitch_UpdateStatic(struct HitSwitchEntry *entry)
{
    if (entry->hitCount >= entry->requiredHits) {
        u16 *state;
        u32 flags;
        u32 mask;
        entry->completed = 1;
        HitSwitch_RegisterCompletedCell(entry);
        state = &entry->collider.state;
        flags = *state;
        mask = 4;
        mask |= flags;
        *state = mask;
        HitSwitch_SetUpdate(entry, HitSwitch_AnimateStaticCompletion);
    }
}
