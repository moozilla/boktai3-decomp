#define ActorStatus_ApplyPendingState sub_081AFF98
#include "global.h"
struct ActorStatus {
    u8 pad0[0x160];
    u32 flags;
    u32 reset164;
    u32 reset168;
    u8 state, previousState, entered, pad16F, phase;
    u8 pad171[7];
    u32 timer;
};
void sub_081AFF98(struct ActorStatus *status)
{
    u8 next;
    if ((u8)(status->state - 1) > 0x1C) {
        status->state = 0;
        return;
    }
    {
        next = status->state;
        {
            u32 mask = 0x100;
            if (!(status->flags & mask)) {
                u32 previous = (u8)(status->previousState - 0x19);
                u8 *phase = &status->phase;
                if (previous <= 1) {
                    if (!*phase)
                        return;
                }
                status->previousState = next;
                status->state = next;
                status->entered = 1;
                *phase = 0;
                status->timer = 0;
                status->reset164 = 0;
                status->reset168 = 0;
            }
        }
    }
}
