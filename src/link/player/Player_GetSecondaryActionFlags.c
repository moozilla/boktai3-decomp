#include "global.h"
#include "player/player.h"

u16 Player_GetSecondaryActionFlags(struct Player *p, u16 mask)
{
    return p->resetValue41A & mask;
}
