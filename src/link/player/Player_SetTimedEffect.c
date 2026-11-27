// COMPILER: old_agbcc
#include "global.h"
/* Player_SetTimedEffect: Stores an effect value at offset 0xAD6 and starts a thirty-two-tick timer; effect interpretation tentative */

void Player_SetTimedEffect(u8 *p, u16 v)
{
    *(u16 *)(p + 0xad6) = v;
    *(u16 *)(p + 0xad8) = 32;
}
