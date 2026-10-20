#include "global.h"

void Player_RecalculateDerivedStats(void *);
void Player_ClampStatsAndSyncStatus(void *);
void sub_0813B7C4(void *);
void PlayerAction_RefreshModifiers(void *);

void Player_RefreshStatsAndVitals(void *p)
{
    Player_RecalculateDerivedStats(p);
    Player_ClampStatsAndSyncStatus(p);
    sub_0813B7C4(p);
    PlayerAction_RefreshModifiers(p);
}
