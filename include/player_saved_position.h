#ifndef GUARD_PLAYER_SAVED_POSITION_H
#define GUARD_PLAYER_SAVED_POSITION_H

#include "game_state.h"
#include "player.h"

static inline void Player_SavePositionAndFacing(struct Player *p)
{
    s16 *saved;
    saved = &gGameState->positionX;
    *saved = p->position.x;
    saved = &gGameState->positionY;
    *saved = p->position.y;
    saved = &gGameState->positionZ;
    *saved = p->position.z;
    saved = &gGameState->unk85E;
    *saved = ((p->facing + 5) & 7) << 5;
}

#endif
