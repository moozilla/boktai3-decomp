#include "global.h"

void PlayerAction_RefreshFlagState(u8 *, u8 *);
void Player_RefreshStatsAndVitals(u8 *);

void Player_RefreshEquipmentWithOptions(u8 *p, s32 unused, s32 flag)
{
    PlayerAction_RefreshFlagState(p, p + 0x32C);
    Player_RefreshStatsAndVitals(p);
    p[0x45E] = 0;
}
