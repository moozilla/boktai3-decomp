#include "global.h"
#include "scene_actor_control_phase.h"

#ifdef NONMATCHING
// fuzzy: 91.3%
// foundry draft (worker best 89.2%); what differs is mostly register allocation and instruction order
#define SceneActorControl_UpdatePausePhase sub_080FA670
void sub_080FA670(struct SceneActorRegistryResetView *actor)
{
    u8 *control = actor->context;
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        void *callback = actor->savedTarget;
        u32 zero = 0;
        actor->field2B2 = 1;
        actor->field2B0 = zero;
        actor->field2B5 = zero;
        actor->target = callback;
        actor->field2C8 = 0;
        {
            u32 *flags = &actor->flags114;
            u32 mask = ~1;
            *flags &= mask;
        }
        actor->word104 = 0;
        control[0xEAA] = 0;
    }
    if (*(s32 *)((u8 *)actor + 0x2AC) > 60) {
        u32 kind = 3;
        actor->state2A8 = 0;
        actor->phase2A9 = 0;
        actor->behavior2AA = kind;
        actor->counter2AC = 0;
        actor->field2B1 = 1;
    }
    ++*(s32 *)((u8 *)actor + 0x2AC);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080FA670);
#endif
