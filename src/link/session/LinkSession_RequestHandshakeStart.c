#include "global.h"
#include "scene/areas/area_effect_link.h"

void LinkSession_RequestHandshakeStart(void)
{
    if (((u8 *)gLinkSession)) {
        ((u8 *)gLinkSession)[0x29] = 1;
    }
}
