#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_chase.h"
void SceneActor_WaitForCategoryAttachment(void);
void SceneActor_StartFlaggedAnimationPhase(struct SceneActorRegistryResetView *actor)
{
    if (Chase_TakeArrived(actor)) {
        struct SceneActorRegistryResetView *context;
        actor->tracking.target = NULL;
        context = actor->context;
        {
            u16 mask = 1;
            struct ChaseFlags40 *flags = (void *)&context->animationFlags65C;
            flags->bits |= mask;
        }
        Chase_SetCtl(actor, SceneActor_WaitForCategoryAttachment, 0x25);
    }
    if (actor->field2B0) {
        Chase_SetBehavior(actor, 2);
    } else {
        Callback_InvokeActor(actor, actor->contactQuery.opaque);
    }
}
