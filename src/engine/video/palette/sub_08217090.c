#include "global.h"
#include "gba/io_reg.h"
void sub_08217090(s32 bg, u32 x, u32 y)
{
    vu16 *reg;
    switch (bg) {
        case 0:
            reg = (vu16 *)REG_ADDR_BG0HOFS;
            break;
        case 1:
            reg = (vu16 *)REG_ADDR_BG1HOFS;
            break;
        case 2:
            reg = (vu16 *)REG_ADDR_BG2HOFS;
            break;
        case 3:
            reg = (vu16 *)REG_ADDR_BG3HOFS;
            break;
        default:
            return;
    }
    *reg++ = x;
    *reg = y;
}
