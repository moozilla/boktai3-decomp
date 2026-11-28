#include "scene/actor/scene_actor_control_phase.h"
#include "engine/word_flags.h"
/* SceneActorControl_UpdateTimedResponseAnimation: Selects heading-dependent animation 11 and sets response flags at the 30 and 60 tick thresholds; response interpretation is provisional. */

void SceneActorControl_GetDirectionalVariant(u32, s32 *, s32 *);

void SceneActorControl_UpdateTimedResponseAnimation(struct ChaseActor *actor, s32 time)
{
    s32 a, b;
    if (actor->base.field2B2)
        actor->base.field2B2 = 0;
    SceneActorControl_GetDirectionalVariant(actor->base.body.direction, &a, &b);
    SceneActorAnimation_SelectExplicitVariant((struct RoomActor *)actor, 11, a, b);
    if (time <= 29)
        HalfwordFlags_Set(0x20, &actor->base.reactionFlags);
    if (time > 59)
        actor->base.stepDone = 1;
}
