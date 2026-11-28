#include "global.h"
#include "objects/deck_cannons/deck_cannon_battery.h"

void DeckCannon_InitMuzzleFlash(struct DeckCannonBattery *, struct DeckCannonShot *,
                                struct ContactPosition *, s32);

void DeckCannon_SpawnMuzzleFlash(struct DeckCannonBattery *battery, struct ContactPosition *origin,
                                 s32 horizontal)
{
    struct DeckCannonShot *shot = NULL;
    s32 i;

    for (i = 0; i < 8; i++) {
        if (!((battery->usedShots >> i) & 1)) {
            shot = &battery->shots[i];
            battery->usedShots |= 1 << i;
            break;
        }
    }
    if (shot)
        DeckCannon_InitMuzzleFlash(battery, shot, origin, horizontal);
}
