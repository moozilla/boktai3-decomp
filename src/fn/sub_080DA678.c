#include "global.h"
#include "mummy.h"
#include "scene_actor_reset.h"
#include "scene_actor_control_motion.h"
#include "scene_actor_animation.h"
#include "display_sequence_inline.h"

// confidence=low: clears mummy motion counters and restarts the directional animation.
void Mummy_ResetMotionState(void *);
void SceneActorMovement_ResolveRoutePosition(void *);

void Mummy_ResetRevivalMotion(struct ChaseActor *actor)
{
    struct Mummy *body = actor->base.context;
    body->targetTick = 0;
    body->pursuitDelay = 0;
    body->flags6D4 = 0;
    body->savedTick = 0;
    body->flags6F2 = 0;
    actor->group.field5C4 = 0;
    body->counter6EE = 0;
    Mummy_ResetMotionState(actor);
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    SceneActorMovement_ResolveRoutePosition(actor);
    {
        struct SceneActorControlMotionDrawable *drawable = actor->base.drawable;
        DisplaySequence_StepState(drawable, &drawable->sequence);
    }
}
