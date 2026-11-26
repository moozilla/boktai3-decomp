// COMPILER: old_agbcc
#include "global.h"
/* Player_SetTimedEffect: Stores an effect value at offset 0xAD6 and starts a thirty-two-tick timer; effect interpretation tentative */
#define Player_SetTimedEffect sub_082436EC

void sub_082436EC(u8 *p, u16 v)
{
    *(u16 *)(p + 0xad6) = v;
    *(u16 *)(p + 0xad8) = 32;
}
