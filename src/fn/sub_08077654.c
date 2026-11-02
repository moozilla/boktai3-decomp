#include "scene_actor_availability.h"
#include "scene_actor_placement.h"

u32 SceneActorPlacement_ConsumeCallbackFlag(struct SceneActorPlacementView *argument)
{
    struct SceneActorPlacementView *actor = argument;
    u32 unavailable = SceneActor_ActionsBlocked((void *)actor);
    if (!unavailable && actor->flags2D6) {
        u8 pending;
        u16 *flags;
        pending = SceneActor_TestPendingFlag((void *)actor, 0x80, &flags);
        if (pending) {
            void *callback = actor->callback23C;
            if (callback)
                Callback_InvokeActor(actor, callback);
            SceneActorPlacement_ClearFlags(flags, ~0x180);
            return 1;
        }
    }
    return 0;
}
