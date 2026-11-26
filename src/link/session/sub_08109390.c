#include "global.h"
#include "scene/areas/area_effect_link.h"

void LinkSession_RequestHandshakeFinish(void)
{
    if (((u8 *)gLinkSession)) {
        ((u8 *)gLinkSession)[0x2B] = 1;
    }
}
