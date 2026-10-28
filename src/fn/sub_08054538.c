#include "hit_switch.h"
#include "player.h"
void HitSwitch_UpdateCompletedBobbing(struct HitSwitchEntry *entry)
{
    if (*(u16 *)((*(u8 **)gPlayers) + 0x28A) == entry->cellIndex) {
        if (entry->sprite.position.y == entry->world.y)
            Sound_PlaySong(691);
        if (entry->sprite.position.y > entry->world.y - 16)
            entry->sprite.position.y -= 2;
    } else if (entry->sprite.position.y < entry->world.y)
        entry->sprite.position.y += 2;
}
