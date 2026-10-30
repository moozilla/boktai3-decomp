#include "global.h"
#include "scene_actor_control_phase.h"

void SceneActorControl_TickPendingTarget(struct SceneActorRegistryResetView *actor)
{
    void *context = *(void **)((u8 *)actor + 0x3D0);

    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        actor->body.direction = *((u8 *)actor + 0xE9);
        {
            void *target = *(void **)((u8 *)actor + 0x228);
            u32 one = 1;
            actor->field2B2 = one;
            actor->field2B0 = 0;
            actor->field2B5 = one;
            actor->target = target;
            actor->field2C8 = 0;
            {
                u32 *flags = &actor->flags114;
                u32 mask = ~1;
                *flags &= mask;
            }
            actor->word104 = 0;
        }
    }
    {
        u32 *counter = (u32 *)((u8 *)actor + 0x2AC);
        if ((s32)*counter > 30) {
            u32 phase = 2;
            actor->state2A8 = 0;
            (*(&actor))->phase2A9 = phase;
            actor->behavior2AA = 0;
            actor->counter2AC = 0;
            actor->field2B1 = 1;
            {
                u32 mask = 8;
                u32 *flags = (u32 *)((u8 *)context + 0x3D4);
                *flags |= mask;
            }
            *(u8 *)((u8 *)context + 0x42A) = 60;
        } else {
            *counter = *counter + 1;
        }
    }
}
