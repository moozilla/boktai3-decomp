#include "global.h"

void Effect_Remove(u32 p);

void ScreenOrbitEffect_DestroySlot(u32 unused, u8 *p)
{
    Effect_Remove((u32)(p + 0x8));
}
