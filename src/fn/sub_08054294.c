#include "hit_switch.h"
void HitSwitch_SetUpdate(struct HitSwitchEntry *entry, void (*update)(struct HitSwitchEntry *))
{
    entry->update = update;
    entry->frame = 0;
}
