#include "hit_switch.h"
#define HitSwitch_UpdateStatic sub_08054354
void HitSwitch_UpdateStatic(struct HitSwitchEntry *entry)
{
    if (entry->hitCount >= entry->requiredHits) {
        u16 *state;
        u32 flags;
        u32 mask;
        entry->completed = 1;
        sub_080542A4(entry);
        state = &entry->collider.state;
        flags = *state;
        mask = 4;
        mask |= flags;
        *state = mask;
        sub_08054294(entry, sub_08054394);
    }
}
