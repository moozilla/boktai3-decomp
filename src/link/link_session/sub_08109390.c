#include "engine/shared/global.h"
#include "engine/shared/area_effect_link.h"

void Registry1745_RequestHandshakeFinish(void)
{
    if (((u8 *)gLinkSession)) {
        ((u8 *)gLinkSession)[0x2B] = 1;
    }
}
