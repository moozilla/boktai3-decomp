#include "global.h"
#include "gba/io_reg.h"

void Video_SetBlendAlpha(u32 firstWeight, u32 secondWeight)
{
    REG_BLDALPHA = (secondWeight << 8) | firstWeight;
}
