#include "engine/shared/global.h"
#include "engine/shared/player.h"
void Player_SetTetherMode(struct Player *p, u32 mode, u32 phase)
{
    p->tetherMode = mode;
    p->tetherPhase = phase;
    p->tetherFrame = 0;
}
