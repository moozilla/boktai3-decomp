#include "global.h"

void Player_IncrementActionSpeedBonus(u8 *p, s32 a)
{
    if (a == 0) {
        p[0x4F6]++;
    }
}
