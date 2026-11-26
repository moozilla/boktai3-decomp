#include "global.h"
void PlayerBarrier_BeginRecovery(u32);
void Player_OnRecoveryActionEnd(u8 *p)
{
    PlayerBarrier_BeginRecovery(*(u32 *)(p + 0xb40));
}
