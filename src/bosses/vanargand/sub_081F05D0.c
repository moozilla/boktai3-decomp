#include "bosses/vanargand/vanargand_head_cooldown.h"
#include "system/game_state/game_state.h"

struct VanargandHeadTarget {
    u8 pad0[0x50];
    struct VanargandHeadActorView *owner;
};
void Sound_PlaySong(u32);
s32 VanargandHead_OnTargetContact(struct VanargandHeadTarget *target, s32 event, s32 amount)
{
    struct VanargandHeadActorView *actor = target->owner;
    s32 level;
    if (event == 0) {
        actor->flashTimer = 15;
        if (amount > 0) {
            level = gGameState->unk876 + 1;
            if (level <= 2) {
                actor->duration = actor->contactDuration;
            } else if (level <= 5) {
                actor->duration = VanargandHead_IncreaseDuration(actor->contactDuration, 2);
            } else {
                actor->duration = VanargandHead_IncreaseDuration(actor->contactDuration, 1);
            }
        }
        if (amount > 0)
            Sound_PlaySong(0x5c9);
    } else if (event != 1)
        return 0;
    return 1;
}
