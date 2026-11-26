#include "engine/shared/global.h"
#include "player/player.h"
#include "player/player_slot_status.h"
void Player_AddControlFlags(struct Player *, u32);
void Player_UpdateFallPose(struct Player *p)
{
    if (p->movementRequestAnimated != 0)
        Player_AddControlFlags(p, 0x1c);
    else
        Player_AddControlFlags(p, 0x1d);
    Player_SetStatusBits(&p->statusFlags, 0x1100);
}
