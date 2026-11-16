#include "scene_actor_reaction.h"
#include "debug.h"
/* SceneActorReaction_ResumeEntryTarget: tentative */

static inline void SetState(struct SceneActorRegistryResetView *actor, u32 s, u32 p, u32 b)
{
    u32 behavior = b;
    actor->base.state2A8 = s;
    actor->base.phase2A9 = p;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}

static inline void SetControl(struct SceneActorRegistryResetView *actor, void *cb, u32 k)
{
    actor->base.field2B2 = 1;
    actor->base.field2B0 = 0;
    actor->base.field2B5 = k;
    actor->base.target = cb;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.flags114;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->base.word104 = 0;
}

void SceneActorReaction_ResumeEntryTarget(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorReactionGroup *group = (void *)&actor->group.common;
    if (SceneActorReaction_TakePrepared(actor)) {
        DEBUG_PRINT();
    }
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 1);
    if (group->timer418 == 0) {
        {
            u32 mask = 0x80;
            actor->base.flags2D4 &= ~mask;
        }
        SetState(actor, 0, 0, 7);
        SetControl(actor, actor->base.entryTarget228, 1);
    }
}
