#include "global.h"
#include "player.h"
void Player_ApplyPeriodicStatChanges(struct Player *);
void Player_NotifyFirstSelectedSlot(u32 slots, u32 flags)
{
    u32 slot;
    struct Player *player;
    if (slots & 1)
        slot = 0;
    else if (slots & 2)
        slot = 1;
    else if (slots & 4)
        slot = 2;
    else if (slots & 8)
        slot = 3;
    else
        return;
    if (flags & 0x17) {
        player = gPlayers[slot];
        if (player && player->kind == 1)
            Player_ApplyPeriodicStatChanges(player);
    }
}
