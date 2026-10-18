#include "global.h"

extern u8 *gUnk_02000580;
void Player_ApplyPeriodicStatChanges(u8 *);

void Player_RefreshLifeState(void)
{
    if (gUnk_02000580 != 0) {
        Player_ApplyPeriodicStatChanges(gUnk_02000580);
    }
}
