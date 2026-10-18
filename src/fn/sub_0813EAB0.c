#include "global.h"

void Player_SetWeaponInstance(u8 *p, u32 v)
{
    *(u32 *)(p + 0xBF0) = v;
}
