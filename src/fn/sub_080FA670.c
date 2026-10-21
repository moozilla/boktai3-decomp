#include "global.h"
#include "scene_actor_control_phase.h"

void SceneActorControl_UpdatePausePhase(struct SceneActorRegistryResetView *actor)
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
    {
        s32 *c = (s32 *)(((u8 *)(*(&actor))) + 0x2AC);
        if (*c > 60) {
            u32 kind = 3;
            actor->state2A8 = 0;
            actor->phase2A9 = 0;
            actor->behavior2AA = kind;
            actor->counter2AC = 0;
            actor->field2B1 = 1;
        }
        ++*c;
    }
}
