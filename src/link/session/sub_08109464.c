#include "global.h"
#include "scene/areas/area_effect_link.h"

s32 LinkSession_GetSessionState(void)
{
    if (((u8 *)gLinkSession) != NULL) {
        return ((u8 *)gLinkSession)[0x1C];
    }
    return 0;
}
