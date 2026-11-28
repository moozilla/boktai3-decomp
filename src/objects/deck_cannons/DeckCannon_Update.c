#include "global.h"
#include "engine/video/display_sprite_playback.h"
#include "objects/deck_cannons/deck_cannon_battery.h"

struct CannonBatteryMessage {
    u32 id;
    u16 *data;
};

s32 MessageQueue_GetMessagesById(u32, struct CannonBatteryMessage **);
void DeckCannon_CreateCannon(struct DeckCannonBattery *, u32, u32, u32, u32);

static inline void RunCallback(void *slot, void *callback)
{
    if (callback)
        Callback_InvokeActor(slot, callback);
}

s32 DeckCannon_Update(struct DeckCannonBattery *battery)
{
    struct CannonBatteryMessage *message;
    s32 count = MessageQueue_GetMessagesById(battery->messageId, &message);
    s32 i;
    u32 bit;
    struct CallbackSlot *cannon;
    struct DeckCannonShot *shot;

    for (; count > 0; count--, message++) {
        u16 *data = message->data;
        if (data[0] == 0)
            DeckCannon_CreateCannon(battery, data[1], data[2], data[3], data[4]);
    }
    for (cannon = battery->cannons, bit = 1, i = 16; i > 0; i--, cannon++, bit <<= 1) {
        RunCallback(cannon, cannon->update);
        if (!cannon->update)
            battery->usedCannons &= ~bit;
    }
    for (shot = battery->shots, bit = 1, i = 8; i > 0; i--, shot++, bit <<= 1) {
        RunCallback(shot, shot->update);
        if (!shot->update)
            battery->usedShots &= ~bit;
        else
            shot->finished = PlayerCue_StepSequenceEnded(&shot->sprite, &shot->sequence);
    }
    return 0;
}
