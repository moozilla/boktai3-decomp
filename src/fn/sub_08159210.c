#include "global.h"
#include "player.h"

void Player_ApplyPeriodicStatChanges(u8 *);

void Player_RefreshLifeState(void)
{
    if ((*(u8 **)gPlayers) != 0) {
        Player_ApplyPeriodicStatChanges((*(u8 **)gPlayers));
    }
}
