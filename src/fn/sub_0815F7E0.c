#include "global.h"

void PlayerBurst_SetStateCallback(u8 *p, s32 v)
{
    *(s32 *)(p + 0x4C4) = v;
    *(u16 *)(p + 0xF2) = 0;
}
