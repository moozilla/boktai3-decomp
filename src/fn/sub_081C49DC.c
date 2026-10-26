#include "global.h"
#include "ram.h"
void Sprite_Remove(void *);
void ShipWheel_OnDestroy(u8 *p)
{
    Sprite_Remove(p + 0x18);
    gShipWheel = 0;
}
