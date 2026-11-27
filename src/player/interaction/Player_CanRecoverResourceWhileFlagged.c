#include "player/player.h"
#include "player/player_slot_status.h"

s32 Player_CanRecoverResourceWhileFlagged(struct Player *p)
{
    if (Player_ReadStatusBits(&p->statusFlags, 0x10) != 0 && p->currentStatB < p->maximumStatB)
        return 1;
    return 0;
}
