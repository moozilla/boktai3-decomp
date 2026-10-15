#include "hit_switch.h"
#define HitSwitch_UpdateBobbing sub_08054410
void HitSwitch_UpdateBobbing(struct HitSwitchEntry *entry)
{
    if (entry->bobPeriod) {
        s32 phase = Div(entry->frame * 255, entry->bobPeriod);
        s32 raw = gUnk_086149C4[phase & 255] * 12;
        s32 offset;
        if (raw >= 0)
            offset = raw >> 12;
        else
            offset = -(-raw >> 12);
        entry->sprite.position.y = (u16)entry->world.y + offset;
        entry->frame++;
        if (entry->frame >= entry->bobPeriod)
            entry->frame = 0;
    }
    if (entry->hitCount >= entry->requiredHits) {
        u16 *state;
        u32 flags;
        entry->completed = 1;
        sub_080542A4(entry);
        state = &entry->collider.state;
        flags = *state;
        {
            u32 mask = 4;
            mask |= flags;
            *state = mask;
        }
        sub_08054294(entry, sub_080544B4);
    }
}
