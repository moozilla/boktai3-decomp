#include "global.h"

void sub_0813EDDC(u8 *, u8 *);
void Player_RefreshStatsAndVitals(u8 *);

void Player_RefreshEquipmentWithOptions(u8 *p, s32 unused, s32 flag)
{
    sub_0813EDDC(p, p + 0x32C);
    Player_RefreshStatsAndVitals(p);
    p[0x45E] = 0;
}
