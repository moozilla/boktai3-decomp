#include "global.h"

extern u8 *gUnk_020004B4;

void Registry1745_NotifyMenuClosed(void)
{
    u8 *p = gUnk_020004B4;
    if (p != 0)
        p[0x1e] = 1;
}
