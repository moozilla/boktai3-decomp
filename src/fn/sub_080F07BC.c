#include "global.h"
#include "scene_actor_reset.h"
void Ivy_UpdateActiveState(void);
void SceneActor_BeginReaction22(struct SceneActorRegistryResetView *actor)
{
    actor->base.state2A8 = 0;
    actor->base.phase2A9 = 0;
    actor->base.behavior2AA = 0;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
    {
        void *callback = Ivy_UpdateActiveState;
        u8 reaction = 0x22;
        actor->base.field2B2 = 1;
        actor->base.field2B0 = 0;
        actor->base.field2B5 = reaction;
        actor->base.target = callback;
    }
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.flags114;
        u32 mask = -2;
        *flags &= mask;
    }
    actor->base.word104 = 0;
}
