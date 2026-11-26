#include "scene/actor/scene_actor_control_helpers.h"
/* SceneActorControl_ClearCollisionLatch: Clears actor flag 0x100 and status bit 8; retains the existing provisional file-local name. */
#define SceneActorControl_ClearCollisionLatch sub_08083F3C
#define SceneActorControl_ClearCollisionLatch sub_08083F3C
void SceneActorControl_ClearCollisionLatch(struct ChaseActor *actor)
{
    struct SceneActorChaseTracking *tracking;
    {
        u16 *flags = &actor->base.motionFlags;
        u32 m = ~0x100;
        m &= *flags;
        *flags = m;
    }
    tracking = &actor->base.tracking;
    {
        u32 m = ~8;
        m &= tracking->flags;
        tracking->flags = m;
    }
}
