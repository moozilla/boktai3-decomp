#include "scene_actor_reaction.h"
/* SceneActorReaction_ResumeEntryTarget: tentative */

static inline void SetState(struct SceneActorRegistryResetView *actor, u32 s, u32 p, u32 b)
{
    u32 behavior = b;
    actor->state2A8 = s;
    actor->phase2A9 = p;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

static inline void SetControl(struct SceneActorRegistryResetView *actor, void *cb, u32 k)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = k;
    actor->target = cb;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = 0;
}

void SceneActorReaction_ResumeEntryTarget(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorReactionGroup *group = (void *)&actor->group.common;
    /* The flag is consumed with nothing to do, as in sub_080EB8CC. */
    if (SceneActorReaction_TakePrepared(actor)) {
    }
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 1);
    if (group->timer418 == 0) {
        {
            u32 mask = 0x80;
            actor->flags2D4 &= ~mask;
        }
        SetState(actor, 0, 0, 7);
        SetControl(actor, actor->entryTarget228, 1);
    }
}
