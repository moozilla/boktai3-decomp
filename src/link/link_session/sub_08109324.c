#include "engine/shared/global.h"
#include "engine/shared/area_effect_link.h"

void sub_08109324(void)
{
    if (((u8 *)gLinkSession) != 0)
        ((u8 *)gLinkSession)[0x25] = 1;
}
