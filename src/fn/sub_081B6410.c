#include "global.h"
void Vanargand_SetPairedMotionParameter(u8 *p, u32 v)
{
    *(u16 *)(p + 0x188) = v;
    *(u16 *)(p + 0x3A4) = v;
}
