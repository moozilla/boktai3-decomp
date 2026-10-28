#include "global.h"
#include "area_effect_link.h"

void sub_08109324(void)
{
    if (((u8 *)gLinkSession) != 0)
        ((u8 *)gLinkSession)[0x25] = 1;
}
