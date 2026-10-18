#include "global.h"
#include "scene_actor_reset.h"
void Ivy_UpdateActiveState(void);
void SceneActor_BeginReaction22(struct SceneActorRegistryResetView *actor)
{
    actor->state2A8 = 0;
    actor->phase2A9 = 0;
    actor->behavior2AA = 0;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
    {
        void *callback = Ivy_UpdateActiveState;
        u8 reaction = 0x22;
        actor->field2B2 = 1;
        actor->field2B0 = 0;
        actor->field2B5 = reaction;
        actor->target = callback;
    }
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        u32 mask = -2;
        *flags &= mask;
    }
    actor->word104 = 0;
}
