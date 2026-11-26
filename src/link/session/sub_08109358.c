#include "global.h"
#include "scene/area_effect/area_effect_link.h"

void Registry1745_RequestHandshakeStart(void)
{
    if (((u8 *)gLinkSession)) {
        ((u8 *)gLinkSession)[0x29] = 1;
    }
}
