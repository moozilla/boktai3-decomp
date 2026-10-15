#include "hit_switch.h"
#define HitSwitch_SetUpdate sub_08054294
void HitSwitch_SetUpdate(struct HitSwitchEntry *entry, void (*update)(struct HitSwitchEntry *))
{
    entry->update = update;
    entry->frame = 0;
}
