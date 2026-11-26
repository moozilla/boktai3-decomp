#include "scene/actor/scene_actor_chase_reaction.h"
#include "global.h"
#include "scene/actor/scene_actor_reaction.h"
#include "debug.h"

static inline u8 TakeReset(struct ChaseActor *actor)
{
    u8 *flag = &actor->base.field2B1;
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
}

void SceneActorControl_EnterTimedState(struct ChaseActor *actor)
{
    if (TakeReset(actor)) {
        DEBUG_PRINT();
    }
    if (!SceneActorChase_MotionFlags(actor, 0x20))
        SceneActorChase_SetBehavior(actor, 10);
    actor->base.counter2AC++;
}
