#include "engine/shared/global.h"
#include "engine/shared/area_effect_link.h"

void Registry1745_NotifyMenuClosed(void)
{
    u8 *p = gLinkSession;
    if (p != 0)
        p[0x1e] = 1;
}
