#include "global.h"
struct ActorStatus {
    u32 displayFlags;
    u8 pad4[0x158];
    u32 mode;
    u32 flags;
    u32 reset164, reset168;
    u8 state, previousState, entered, pad16F, phase;
    u8 pad171[3];
    s16 duration;
    u8 pad176[2];
    u32 timer;
};
struct ActorStatusOwner {
    u8 pad0[0x5B4];
    u32 activeMask;
};
void ActorStatus_Deactivate(struct ActorStatusOwner *owner, struct ActorStatus *status)
{
    u32 mask = 0x100;
    if (!(status->flags & mask)) {
        u32 previous = (u8)(status->previousState - 0x19);
        u8 *phase = &status->phase;
        if (previous > 1 || *phase) {
            status->previousState = 0;
            status->state = 0;
            status->entered = 1;
            *phase = 0;
            status->timer = 0;
            status->reset164 = 0;
            status->reset168 = 0;
        }
    }
    status->displayFlags |= 1;
    if (status->mode == 0)
        owner->activeMask &= ~1;
    else
        owner->activeMask &= ~2;
    status->duration = 0;
}
