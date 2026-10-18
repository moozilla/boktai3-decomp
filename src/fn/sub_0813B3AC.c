// COMPILER: old_agbcc
#include "global.h"
/* Player_SetEventFlags: Sets bits in the player flag word at offset B34; event interpretation is tentative */
#define Player_SetEventFlags sub_0813B3AC

void sub_0813B3AC(u8 *p, u32 bits)
{
    *(u16 *)(p + 0xb34) = bits | *(u16 *)(p + 0xb34);
}
