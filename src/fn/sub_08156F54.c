#include "tether_actor.h"

s32 TetherActor_TestMatchingStatus(u32, u32, u32, u32);

static inline u32 TetherAcceptsActionInput(void)
{
    if (!gTetherActor)
        return 0;
    switch (gTetherActor->state) {
        case 1:
        case 0x11:
            return 1;
    }
    return 0;
}

s32 Player_TestTetherActionInput(struct Player *p)
{
    if (*p->inputKeys & 1) {
        if (!TetherAcceptsActionInput())
            return 0;
        if (TetherActor_TestMatchingStatus(p->tetherStatusTag, p->tetherStatusFlags,
                                           gTetherActor->statusTag, gTetherActor->statusFlags))
            return 1;
    }
    return 0;
}
