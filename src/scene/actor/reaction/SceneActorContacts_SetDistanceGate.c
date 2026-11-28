#include "scene/collision/scene_actor_contact_list.h"
#include "scene/motion/scene_actor_chase_shared.h"
void SceneActorContacts_SetDistanceGate(struct ChaseActor *actor,
                                        struct SceneActorContactGateView *gate, u32 metric,
                                        u32 threshold, u32 mode, u32 flags)
{
    flags &= 6;
    if (flags) {
        if (!threshold && !mode) {
            gate->enabled = 1;
        } else {
            u32 result = SceneActorChase_TrackingFlags(&actor->base.tracking, 4);
            if (result || (mode != 2 && metric <= threshold))
                gate->enabled = 1;
            else
                gate->enabled = result;
        }
    } else {
        gate->enabled = flags;
    }
}
