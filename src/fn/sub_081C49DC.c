#include "global.h"
extern u32 gUnk_0200026C;
void Sprite_Remove(void *);
void ShipWheel_OnDestroy(u8 *p)
{
    Sprite_Remove(p + 0x18);
    gUnk_0200026C = 0;
}
