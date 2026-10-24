#include "global.h"

// confidence=low: ActorController_ForceAction (sub_081850B4) gated on the actor's ready flag.
struct ArmorController {
    u8 f0[0x196];
    u16 flags;
    u8 f1[0xB32 - 0x198];
    u16 state;
    u8 f2[0xB3F - 0xB34];
    u8 action;
    u8 phase;
    u8 ready;
    u8 timer;
};
s32 Armor_TryStartAction(struct ArmorController *p, s32 action)
{
    s32 started;
    if (p->ready) {
        p->action = action;
        p->phase = 0;
        p->timer = 0;
        p->flags |= 4;
        p->state &= 0xFFFD;
        started = 1;
    } else {
        started = 0;
    }
    return started;
}
