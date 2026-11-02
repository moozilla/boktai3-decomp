#include "slot7_cooldown.h"
#include "game_state.h"

struct Slot7Target {
    u8 pad0[0x50];
    struct Slot7Actor *owner;
};
void Sound_PlaySong(u32);
s32 SingletonSlot7_OnTargetContact(struct Slot7Target *target, s32 event, s32 amount)
{
    struct Slot7Actor *actor = target->owner;
    s32 level;
    if (event == 0) {
        actor->flashTimer = 15;
        if (amount > 0) {
            level = gGameState->unk876 + 1;
            if (level <= 2) {
                actor->duration = actor->contactDuration;
            } else if (level <= 5) {
                actor->duration = Slot7_IncreaseDuration(actor->contactDuration, 2);
            } else {
                actor->duration = Slot7_IncreaseDuration(actor->contactDuration, 1);
            }
        }
        if (amount > 0)
            Sound_PlaySong(0x5c9);
    } else if (event != 1)
        return 0;
    return 1;
}
