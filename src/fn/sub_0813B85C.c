#include "global.h"

void Player_InitializeStatsFromScript(void *);
void sub_0813B7C4(void *);

void Player_InitVitalsAndCollisionFromScript(void *p)
{
    Player_InitializeStatsFromScript(p);
    sub_0813B7C4(p);
}
