#include "global.h"
#include "objects/deck_cannons/deck_cannon_battery.h"

void DeckCannon_DestroyMuzzleFlash(struct DeckCannonShot *shot)
{
    Sprite_Remove(&shot->sprite);
}
