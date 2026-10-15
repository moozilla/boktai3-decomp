#include "hit_switch.h"
#define HitSwitch_UpdateCompletedBobbing sub_08054538
extern u8 *gUnk_02000580;
void HitSwitch_UpdateCompletedBobbing(struct HitSwitchEntry *entry)
{
    if (*(u16 *)(gUnk_02000580 + 0x28A) == entry->cellIndex) {
        if (entry->sprite.position.y == entry->world.y)
            Sound_PlaySong(691);
        if (entry->sprite.position.y > entry->world.y - 16)
            entry->sprite.position.y -= 2;
    } else if (entry->sprite.position.y < entry->world.y)
        entry->sprite.position.y += 2;
}
