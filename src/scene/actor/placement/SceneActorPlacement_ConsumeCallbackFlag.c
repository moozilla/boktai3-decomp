#include "scene/motion/scene_actor_availability.h"
#include "scene/actor/scene_actor_placement.h"

u32 SceneActorPlacement_ConsumeCallbackFlag(struct RoomActor *actor)
{
    if (!SceneActor_ActionsBlocked((void *)actor) && actor->reactionFlags) {
        u16 *flags;
        if (SceneActor_TestPendingFlag((void *)actor, 0x80, &flags)) {
            void *callback = actor->reactionGate;
            if (callback)
                Callback_InvokeActor(actor, callback);
            SceneActorPlacement_ClearFlags(flags, ~0x180);
            return 1;
        }
    }
    return 0;
}
