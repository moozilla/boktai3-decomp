#include "hit_switch.h"
s32 HitSwitch_UpdateManager(struct HitSwitchManager *manager)
{
    s32 i;
    HitSwitch_UpdateSharedFlash(manager);
    for (i = 0; i < manager->count; i++) {
        struct HitSwitchEntry *entry = &manager->entries[i];
        entry->update(entry);
        if (entry->hitCooldown) {
            entry->hitCooldown--;
            if (entry->hitCooldown) {
                entry->animation.paletteIndex = 214;
                entry->animation.palette = manager->palette;
            } else
                AnimData_SelectPaletteBank(&entry->animation, 213);
        }
    }
    return 0;
}
