#include "global.h"

void Player_RecalculateDerivedStats(void *);
void sub_0813B5F8(void *);
void sub_0813B7C4(void *);
void PlayerAction_RefreshModifiers(void *);

void Player_RefreshStatsAndVitals(void *p)
{
    Player_RecalculateDerivedStats(p);
    sub_0813B5F8(p);
    sub_0813B7C4(p);
    PlayerAction_RefreshModifiers(p);
}
