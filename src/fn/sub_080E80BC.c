#include "global.h"
#include "scene_actor_chase.h"
// confidence=low: retries the saved pursuit goal and selects idle behavior on failure.
u8 Mimic_TryPursuitRoute(struct SceneActorRegistryResetView *actor)
{
    u32 output;
    u32 result = Chase_TryRoute(actor, *(void **)((u8 *)actor + 0x5D0), &actor->body.position,
                                (void *)((u8 *)actor + 0x650));
    if (result)
        output = 1;
    else {
        u32 behavior = 17;
        actor->state2A8 = result;
        actor->phase2A9 = result;
        actor->behavior2AA = behavior;
        actor->counter2AC = result;
        actor->field2B1 = 1;
        output = 0;
    }
    return output;
}
