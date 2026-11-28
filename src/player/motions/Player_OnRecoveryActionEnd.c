#include "global.h"
#include "player/player.h"

void PileTrap_BeginRecovery(void *);

void Player_OnRecoveryActionEnd(struct Player *p)
{
    PileTrap_BeginRecovery(p->actionRequestEffect);
}
