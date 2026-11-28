#include "global.h"
#include "objects/deck_cannons/deck_cannon_battery.h"

/* Resets the -b values (default 30, 10, 180) and clears sixteen cannon and
   eight shot slots. */
u32 DeckCannon_ReadOptions(struct DeckCannonBattery *battery)
{
    s32 i;

    battery->usedCannons = 0;
    battery->usedShots = 0;
    battery->attack = 30;
    battery->attackFlags = 10;
    battery->delay = 180;
    if (Script_SeekToKeyword('b')) {
        battery->attack = Script_GetValue();
        battery->attackFlags = Script_GetValue();
        battery->delay = Script_GetValue();
    }
    for (i = 0; i < 16; i++) {
        struct CallbackSlot *cannon = &battery->cannons[i];
        cannon->update = 0;
        cannon->destroy = 0;
    }
    for (i = 0; i < 8; i++) {
        struct DeckCannonShot *shot = &battery->shots[i];
        shot->update = 0;
        shot->destroy = 0;
    }
    return 0;
}
