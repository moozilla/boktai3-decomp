#include "global.h"
#include "scene/area_effect/area_effect_link.h"

s32 Registry1745_GetSessionState(void)
{
    if (((u8 *)gLinkSession) != NULL) {
        return ((u8 *)gLinkSession)[0x1C];
    }
    return 0;
}
