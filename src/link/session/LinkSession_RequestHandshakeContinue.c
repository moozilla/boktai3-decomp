#include "global.h"
#include "scene/areas/area_effect_link.h"

void LinkSession_RequestHandshakeContinue(void)
{
    if (((u8 *)gLinkSession)) {
        ((u8 *)gLinkSession)[0x2A] = 1;
    }
}
