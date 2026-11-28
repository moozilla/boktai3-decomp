#include "global.h"
#include "gba/io_reg.h"
void sub_082170D4(s32 bg, u32 x, u32 y)
{
    vu16 *reg;
    switch (bg) {
        case 0:
        case 1:
            return;
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
