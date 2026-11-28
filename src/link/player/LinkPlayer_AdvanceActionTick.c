#include "global.h"
#include "player/player.h"

void LinkPlayer_AdvanceActionTick(struct Player *p)
{
    if (p->inputKeys[1] & 0xF0)
        p->facingChangeEffects++;
}
