#include "global.h"
#include "gba/io_reg.h"

void Video_SetWindowLayerMasks(u32 window0, u32 window1, u32 outside, u32 objectWindow)
{
    REG_WININ = (window1 << 8) | window0;
    REG_WINOUT = (objectWindow << 8) | outside;
}
