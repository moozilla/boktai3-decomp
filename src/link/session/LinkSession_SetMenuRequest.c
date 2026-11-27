#include "global.h"
#include "scene/areas/area_effect_link.h"

void LinkSession_SetMenuRequest(u32 v)
{
    if ((*(u32 *)&gLinkSession) != 0) {
        *(u8 *)((*(u32 *)&gLinkSession) + 0x23) = v;
    }
}
