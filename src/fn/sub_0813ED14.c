#include "global.h"

void Player_SetWeaponMode(u8 *p, u8 v)
{
    p[0xC18] = v;
}
