#include "global.h"
#include "objects/deck_cannons/deck_cannon_battery.h"

void DeckCannon_DestroyProjectile(struct DeckCannonShot *shot)
{
    Collider_Remove(&shot->collider);
    Sprite_Remove(&shot->sprite);
}
