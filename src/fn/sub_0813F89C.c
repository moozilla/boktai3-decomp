#include "global.h"

s32 Player_HasRequiredEnergy(u8 *p, s32 v)
{
    if ((s32) * (u16 *)(p + 0x428) < v) {
        return 0;
    }
    return 1;
}
