#include "monsters/cannons/wind_cannon.h"
u32 WindCannonManager_DestroySlot(struct FanCannonManager *a, struct FanCannon *b, u32 n)
{
    b->sprite.flags |= 1;
    Sprite_Remove(&b->sprite);
    a->occupiedSlots &= ~(1 << n);
}
