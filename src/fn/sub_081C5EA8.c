#include "global.h"

s32 sub_081C5EA8(s32 n)
{
    s32 r;
    switch (n) {
    case 1: r = 0x26; break;
    case 2: r = 0x25; break;
    case 3: r = 0x24; break;
    case 4: r = 0x23; break;
    case 5: r = 0x22; break;
    default: r = -1; break;
    }
    return r;
}
