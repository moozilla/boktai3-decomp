#include "global.h"

u32 Armor_IsSequenceIncomplete(u8 *p)
{
    u32 r = 0;
    if (p[0x5C4] < p[0x5C3]) {
        r = 1;
    }
    return r;
}
