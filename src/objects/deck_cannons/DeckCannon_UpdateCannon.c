#include "global.h"
#include "objects/deck_cannons/deck_cannon_battery.h"

extern void (*const gUnk_0824F6C8[])(struct CallbackSlot *);

void DeckCannon_UpdateCannon(struct CallbackSlot *cannon)
{
    Callback_InvokeActor(cannon, gUnk_0824F6C8[cannon->state]);
    Collider_SetPosition(&cannon->collider, &cannon->position, 0);
}
