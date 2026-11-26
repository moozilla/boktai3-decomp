#include "global.h"
void PileTrap_BeginRecovery(u32);
void Player_OnRecoveryActionEnd(u8 *p)
{
    PileTrap_BeginRecovery(*(u32 *)(p + 0xb40));
}
