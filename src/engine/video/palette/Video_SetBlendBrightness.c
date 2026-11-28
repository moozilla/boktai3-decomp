#include "global.h"
#include "gba/io_reg.h"

void Video_SetBlendBrightness(u32 level)
{
    REG_BLDY = level;
}
