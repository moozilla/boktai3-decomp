#include "global.h"
#include "area_effect_link.h"

void Registry1745_SetMenuRequest(u32 v)
{
    if ((*(u32 *)&gLinkSession) != 0) {
        *(u8 *)((*(u32 *)&gLinkSession) + 0x23) = v;
    }
}
