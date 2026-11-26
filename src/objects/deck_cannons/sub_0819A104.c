#include "global.h"

u32 Sprite_Remove(u8 *);

void CannonBattery_DestroyMuzzleFlash(u8 *p)
{
    Sprite_Remove(p + 0x24);
}
