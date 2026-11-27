#include "global.h"
#include "player/player.h"

// confidence=low: alternate action handlers use this copy of the facing-fold helper.

void Player_UpdateActionFacingSide(struct Player *player)
{
    u32 direction = player->facing;
    if (direction > 4) {
        u32 side = 8 - direction;
        player->heading = side;
        player->side = 1;
    } else {
        player->heading = direction;
        player->side = 0;
    }
}
