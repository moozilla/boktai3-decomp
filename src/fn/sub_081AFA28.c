#include "paired_scene_actor.h"
#include "global.h"

struct ActorStatusOwner {
    u8 pad[0x50];
    struct PairedSceneHalf *status;
};
static inline u32 masked(u32 *p, u32 mask)
{
    return *p & mask;
}
s32 ActorStatus_BeginTimedState(struct ActorStatusOwner *owner, s32 cancel, s32 duration)
{
    struct PairedSceneHalf *status = owner->status;
    if (cancel == 0) {
        if (status->mode == 0)
            duration = status->health;
        else
            duration -= *(u16 *)status->unknown18A;
        if (duration > 0) {
            u32 mask = 0x100;
            if (!masked(&status->actionFlags, mask)) {
                status->state = 0x15;
                status->finished = 0;
            }
            status->specialDamage = duration;
            status->shake = 15;
        }
    } else {
        u32 state, phase;
        status->actionFlags = masked(&status->actionFlags, ~0x100);
        state = 0x16;
        phase = 0;
        status->state = state;
        status->finished = phase;
    }
    return 1;
}
