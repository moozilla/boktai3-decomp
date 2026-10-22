#include "global.h"
extern void *gUnk_020001AC;

u8 *sub_08103F24(u8 *p)
{
    if (gUnk_020001AC == 0)
        return 0;
    return p + 0x10c;
}
