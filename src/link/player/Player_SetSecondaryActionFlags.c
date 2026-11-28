#include "global.h"
#include "player/player.h"

void Player_SetSecondaryActionFlags(struct Player *p, u32 mask)
{
    p->resetValue41A |= mask;
}
