#include "global.h"
struct ActorExtensionState {
    u8 unknown0[0x46A];
    u16 state;
    u32 timer;
    u8 active;
};
/* The original ABI leaves r0 incidental; callers must not consume a defined result. */
u32 ActorExtension_ResetState(struct ActorExtensionState *p)
{
    p->state = 0;
    p->active = 0;
    p->timer = 0;
}
