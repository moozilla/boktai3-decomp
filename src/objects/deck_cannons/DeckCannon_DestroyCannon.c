#include "global.h"
#include "objects/deck_cannons/deck_cannon_battery.h"

void DeckCannon_DestroyCannon(struct CallbackSlot *cannon)
{
    if (cannon->spriteRegistered)
        Sprite_Remove(&cannon->flags);
}
