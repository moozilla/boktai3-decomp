#include "global.h"

void Collider_Remove(u8 *);
u32 Sprite_Remove(u8 *);

void CannonBattery_DestroyProjectile(u8 *p)
{
    Collider_Remove(p + 0x60);
    Sprite_Remove(p + 0x24);
}
