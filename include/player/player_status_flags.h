#ifndef GUARD_PLAYER_STATUS_FLAGS_H
#define GUARD_PLAYER_STATUS_FLAGS_H

#include "player/player.h"

static inline void Player_AddStatusFlags(struct Player *player, u32 mask)
{
    player->statusFlags |= mask;
}

#endif
