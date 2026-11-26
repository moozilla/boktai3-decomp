#include "engine/shared/global.h"
#include "engine/shared/area_effect_link.h"

bool32 sub_081093DC(void)
{
    if (((u8 *)gLinkSession) == 0 || ((u8 *)gLinkSession)[0x1e] == 0)
        return FALSE;
    return TRUE;
}
