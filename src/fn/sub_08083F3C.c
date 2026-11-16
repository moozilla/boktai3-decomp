#include "scene_actor_control_helpers.h"
/* SceneActorControl_ClearCollisionLatch: Clears actor flag 0x100 and status bit 8; retains the existing provisional file-local name. */
#define SceneActorControl_ClearCollisionLatch sub_08083F3C
#define SceneActorControl_ClearCollisionLatch sub_08083F3C
void SceneActorControl_ClearCollisionLatch(struct ChaseActor *actor)
{
    u8 *status;
    {
        u16 *flags = &actor->base.motionFlags;
        u32 m = ~0x100;
        m &= *flags;
        *flags = m;
    }
    status = (u8 *)actor + 0xDC;
    {
        u32 m = ~8;
        m &= status[15];
        status[15] = m;
    }
}
