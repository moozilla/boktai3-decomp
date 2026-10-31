#include "player_cue.h"
void Player_OnStateOneEvent(u32 a, u32 b, struct Player *p)
{
    if (p->actionMode == 1)
        gGameState->unk600 += 4;
}
