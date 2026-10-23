#include "global.h"

#define Player_UpdateFacingSide sub_0813ADD0

struct FacingState {
    u8 pad[0x3A2];
    u8 side;
    u8 flipped;
    u8 direction;
};

void Player_UpdateFacingSide(struct FacingState *player)
{
    u32 direction = player->direction;
    if (direction > 4) {
        u32 side = 8 - direction;
        player->side = side;
        player->flipped = 1;
    } else {
        player->side = direction;
        player->flipped = 0;
    }
}
