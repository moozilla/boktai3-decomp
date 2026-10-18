#include "global.h"

void Player_RefreshEquipmentWithOptions(u8 *p, s32 unused, s32 flag);

void Player_RefreshEquipment(u8 *p, s32 q)
{
    Player_RefreshEquipmentWithOptions(p, q, -1);
}
