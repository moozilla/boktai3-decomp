#include "global.h"
#define ActorExtension_ResetState sub_08179EB8
struct ActorExtensionState {
    u8 unknown0[0x46A];
    u16 state;
    u32 timer;
    u8 active;
};
/* The original ABI leaves r0 incidental; callers must not consume a defined result. */
u32 sub_08179EB8(struct ActorExtensionState *p)
{
    p->state = 0;
    p->active = 0;
    p->timer = 0;
}
