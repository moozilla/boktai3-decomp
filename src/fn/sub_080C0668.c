#include "global.h"
#include "scene_actor_reaction.h"

struct SceneActorApproachActionContext {
    u8 unknown0[0x6F0];
    u16 timer;
    u8 unknown6F2[6];
    u16 duration;
};
void SceneActorReaction_TickQueuedContact(void);
void SceneActorReaction_TickContactAnimation(void);
void Vorn_SpawnHazard(struct SceneActorRegistryResetView *, s32);
void Skeleton_SelectDistanceReaction(struct SceneActorRegistryResetView *);
static inline u8 TakeReady(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2B1) {
        actor->field2B1 = 0;
        return 1;
    }
    return 0;
}
static inline void ClearFlags(u16 *p, u32 mask)
{
    *p &= mask;
}
static inline void SetControl(struct SceneActorRegistryResetView *actor, void *fn)
{
    u8 code = 3;
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = code;
    actor->target = fn;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        s32 mask = -2;
        *flags &= mask;
    }
    actor->word104 = 0;
}
void SceneActorApproach_RunCategoryAction(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorApproachActionContext *context = actor->context;
    if (TakeReady(actor)) {
        SceneActorReaction_EnableMotionFlags(0x100, &actor->flags2D4);
        {
            if (actor->category == 7)
                SetControl(actor, SceneActorReaction_TickQueuedContact);
            else
                SetControl(actor, SceneActorReaction_TickContactAnimation);
        }
        Vorn_SpawnHazard(actor, 1);
        context->timer = context->duration;
    }
    if (actor->field2B0) {
        ClearFlags(&actor->flags2D4, ~0x100);
        Skeleton_SelectDistanceReaction(actor);
    } else
        actor->counter2AC++;
}
