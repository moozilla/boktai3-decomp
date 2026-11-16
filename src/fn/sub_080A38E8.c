#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_control_motion.h"
#include "scene_actor_animation.h"
#include "display_sequence_inline.h"

/* SceneActorRoute_ResetStep: tentative */

struct SpiderResetContext {
    u8 unknown0[0x628];
    u16 field628, field62A, field62C;
};

void SceneActorMovement_ResolveRoutePosition(void *);
void SceneActorRoute_ResetStep(struct ChaseActor *actor)
{
    struct SpiderResetContext *context = actor->base.context;
    context->field628 = 0;
    context->field62A = 0;
    context->field62C = 0;
    SceneActorMovement_ResolveRoutePosition(actor);
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    {
        struct SceneActorControlMotionDrawable *drawable = actor->base.drawable;
        DisplaySequence_StepState(drawable, &drawable->sequence);
    }
}
