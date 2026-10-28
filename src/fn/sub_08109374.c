#include "global.h"
#include "area_effect_link.h"

void Registry1745_RequestHandshakeContinue(void)
{
    if (((u8 *)gLinkSession)) {
        ((u8 *)gLinkSession)[0x2A] = 1;
    }
}
