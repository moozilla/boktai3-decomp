#include "global.h"
#include "objects/deck_cannons/deck_cannon_battery.h"

s32 DeckCannon_Update(struct DeckCannonBattery *);
s32 sub_0819A414(struct DeckCannonBattery *);
s32 DeckCannon_ReadOptions(struct DeckCannonBattery *);

struct DeckCannonBattery *DeckCannon_Create(u16 messageId)
{
    struct DeckCannonBattery *battery = Task_Create(8, sizeof(*battery));

    if (battery) {
        battery->messageId = messageId;
        Task_SetCallbacks(battery, (void (*)(void *))DeckCannon_Update,
                          (void (*)(void *))sub_0819A414);
        if (DeckCannon_ReadOptions(battery) < 0) {
            Task_Kill(battery);
            return 0;
        }
    }
    return battery;
}
