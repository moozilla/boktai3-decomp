#include "scene/collision/scene_actor_contact_list.h"
#include "scene/motion/scene_actor_chase_shared.h"
void SceneActorContacts_SetDistanceGate(struct ChaseActor *actor,
                                        struct SceneActorContactGateView *gate, u32 metric,
                                        u32 threshold, u32 mode, u32 flags)
{
    flags &= 6;
    if (flags) {
        u32 result;
        if (!threshold && !mode)
            goto yes;
        result = SceneActorChase_TrackingFlags(&actor->base.tracking, 4);
        if (result)
            goto yes;
        if (mode == 2)
            goto write;
        if (metric > threshold)
            goto write;
    yes:
        result = 1;
    write:
        do {
            gate->enabled = result;
        } while (0);
    } else {
        gate->enabled = flags;
    }
}
