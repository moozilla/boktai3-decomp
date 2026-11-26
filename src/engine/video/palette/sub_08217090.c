#include "global.h"
void sub_08217090(s32 bg, u32 x, u32 y)
{
    vu16 *reg;
    switch (bg) {
        case 0:
            reg = (vu16 *)0x4000010;
            break;
        case 1:
            reg = (vu16 *)0x4000014;
            break;
        case 2:
            reg = (vu16 *)0x4000018;
            break;
        case 3:
            reg = (vu16 *)0x400001C;
            break;
        default:
            return;
    }
    *reg++ = x;
    *reg = y;
}
