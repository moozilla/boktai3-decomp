#include "hit_switch.h"
#define HitSwitch_UpdateManager sub_080546B8
s32 HitSwitch_UpdateManager(struct HitSwitchManager *manager)
{
    s32 i;
    sub_080545A4(manager);
    for (i = 0; i < manager->count; i++) {
        struct HitSwitchEntry *entry = &manager->entries[i];
        entry->update(entry);
        if (entry->hitCooldown) {
            entry->hitCooldown--;
            if (entry->hitCooldown) {
                entry->animation.initial = 214;
                entry->animation.palette = manager->palette;
            } else
                AnimData_SelectTiles(&entry->animation, 213);
        }
    }
    return 0;
}
