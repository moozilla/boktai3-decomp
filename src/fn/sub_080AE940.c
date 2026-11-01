#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_chase.h"
struct VornApproachContext {
    u8 unknown0[0x6C8];
    u32 minimumDistance, maximumDistance;
    u8 unknown6D0[4];
    u16 flags;
};
/* Vorn_UpdateApproachRange: tentative */
u8 SceneActor_IsFacingNearbyTarget(struct SceneActorRegistryResetView *);
void Vorn_SelectRetreatRoute(struct SceneActorAnimationView *, u32);
s32 Callback_InvokeActor(void *, void *);
void Vorn_UpdateApproachRange(struct SceneActorRegistryResetView *actor)
{
    struct VornApproachContext *context = actor->context;
    u8 *group = (u8 *)&actor->group.common;
    if (SceneActorReaction_TakePrepared(actor))
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    else if (actor->tracking.distance > context->maximumDistance ||
             actor->tracking.distance < context->minimumDistance) {
        Vorn_SelectRetreatRoute((void *)actor, 1);
    } else if (SceneActor_IsFacingNearbyTarget(actor)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        if (!Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->body.position,
                            &((struct ChaseTarget *)actor->tracking.target)->position))
            Callback_InvokeActor(actor, *(void **)(group + 0x224));
        else {
            {
                struct ChaseFlags40 *flags = (void *)&context->flags;
                s32 mask = ~0x40;
                flags->bits &= mask;
            }
            Chase_SetBehavior(actor, 21);
        }
    }
}
