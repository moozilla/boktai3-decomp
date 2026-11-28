#include "global.h"
#include "objects/deck_cannons/deck_cannon_battery.h"

void DeckCannon_InitCannon(struct CallbackSlot *, s32, s32, s32, s32);

struct CallbackSlot *DeckCannon_CreateCannon(struct DeckCannonBattery *battery, s32 x, s32 y, s32 z,
                                             s32 flip)
{
    struct CallbackSlot *cannon = NULL;
    s32 i;

    for (i = 0; i < 16; i++) {
        if (!((battery->usedCannons >> i) & 1)) {
            cannon = &battery->cannons[i];
            battery->usedCannons |= 1 << i;
            break;
        }
    }
    if (!cannon)
        return NULL;
    cannon->owner = battery;
    DeckCannon_InitCannon(cannon, x, y, z, flip);
    return cannon;
}
