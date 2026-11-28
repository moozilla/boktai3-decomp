#include "global.h"
#include "gba/io_reg.h"

void Video_SetBlendControl(u32 effect, u32 firstTargets, u32 secondTargets)
{
    REG_BLDCNT = firstTargets | (secondTargets << 8) | (effect << 6);
}
