#include "global.h"

extern u32 gUnk_030053F4;

u32 sub_0814DD3C(u32 x)
{
    u32 r;
    if (gUnk_030053F4 & 0x1000) {
        r = 0;
    } else {
        switch (x) {
        case 1:
            r = 0x40;
            break;
        case 2:
            r = 0x200004;
            break;
        case 3:
            r = 0x400008;
            break;
        case 4:
            r = 0x10;
            break;
        case 5:
            r = 0x20;
            break;
        case 0:
        case 7:
            r = 1;
            break;
        case 8:
            r = 2;
            break;
        default:
            r = 0;
            break;
        }
    }
    return r;
}
