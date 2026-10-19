#define ActorStatus_BeginTimedState sub_081AFA28
#include "global.h"

struct ActorStatus {
    u8 pad0[0x15C];
    u32 mode;
    u32 flags;
    u8 pad164[8];
    u8 state;
    u8 pad16D[3];
    u8 phase;
    u8 pad171[3];
    s16 duration;
    u8 pad176[0xC];
    s16 remaining;
    u8 pad184[6];
    u16 elapsed;
    u8 pad18C[0x10];
    u32 timer;
};
struct ActorStatusOwner {
    u8 pad[0x50];
    struct ActorStatus *status;
};
static inline u32 masked(u32 *p, u32 mask)
{
    return *p & mask;
}
s32 sub_081AFA28(struct ActorStatusOwner *owner, s32 cancel, s32 duration)
{
    struct ActorStatus *status = owner->status;
    if (cancel == 0) {
        if (status->mode == 0)
            duration = status->duration;
        else
            duration -= status->elapsed;
        if (duration > 0) {
            u32 mask = 0x100;
            if (!masked(&status->flags, mask)) {
                status->state = 0x15;
                status->phase = 0;
            }
            status->remaining = duration;
            status->timer = 15;
        }
    } else {
        u32 state, phase;
        status->flags = masked(&status->flags, ~0x100);
        state = 0x16;
        phase = 0;
        status->state = state;
        status->phase = phase;
    }
    return 1;
}
