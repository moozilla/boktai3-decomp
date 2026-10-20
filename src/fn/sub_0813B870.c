#include "global.h"

void Player_ClampStatsAndSyncStatus(void *);
void sub_0813B7C4(void *);

void Player_RefreshVitalsAndCollision(void *p)
{
    Player_ClampStatsAndSyncStatus(p);
    sub_0813B7C4(p);
}
