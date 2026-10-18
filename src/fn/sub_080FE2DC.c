#include "global.h"

extern u8 gUnk_08606468[];

void Axe_InitActionCallbacks(u8 *p)
{
    *(u8 **)(p + 0x28c) = gUnk_08606468;
}
