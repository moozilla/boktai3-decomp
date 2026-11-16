#include "global.h"
#include "dog.h"
#include "scene_actor_reset.h"
#include "scene_actor_chase.h"
void SceneActor_WaitForCategoryAttachment(void);
void SceneActor_StartFlaggedAnimationPhase(struct Dog *actor)
{
    if (Chase_TakeArrived(&actor->chase)) {
        struct Dog *context;
        actor->chase.base.tracking.target = NULL;
        context = actor->chase.base.context;
        {
            u16 mask = 1;
            struct ChaseFlags40 *flags = (void *)&context->collider.reactionFlags;
            flags->bits |= mask;
        }
        Chase_SetCtl(&actor->chase, SceneActor_WaitForCategoryAttachment, 0x25);
    }
    if (actor->chase.base.stepDone) {
        Chase_SetBehavior(&actor->chase, 2);
    } else {
        Callback_InvokeActor(actor, actor->chase.base.contactQuery.opaque);
    }
}
