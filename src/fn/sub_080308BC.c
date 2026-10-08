#include "global.h"

extern u32 gUnk_020000DC;
u32 sub_080308BC(void)
{
    u32 x = gUnk_020000DC;
    if (x != 0)
        x = 1;
    return x;
}
