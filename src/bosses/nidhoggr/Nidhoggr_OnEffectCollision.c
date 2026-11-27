#include "global.h"

void Nidhoggr_OnEffectCollision(u32 a, u32 b, u8 *p)
{
    *(p + 0x180) = 0xff;
}
