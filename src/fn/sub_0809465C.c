#include "global.h"

extern u8 gUnk_08605D88[];

void Bee_InitBehaviorHandlers(u8 *p)
{
    *(u8 **)(p + 0x28c) = gUnk_08605D88;
}
