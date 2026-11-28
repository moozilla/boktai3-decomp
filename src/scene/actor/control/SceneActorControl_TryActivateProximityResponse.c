#include "global.h"
#include "player/player.h"
#include "scene/motion/scene_actor_chase_shared.h"

u32 SceneActorControl_TryActivateProximityResponse(struct ChaseActor *actor)
{
    struct SceneActorChaseTracking *tracking;
    if (!SceneActorChase_TrackingFlags(&actor->base.tracking, 2))
        goto zero;
    tracking = &actor->base.tracking;
    {
        s32 mask = ~2;
        mask &= tracking->flags;
        tracking->flags = mask;
    }
    if (abs(actor->base.body.position.y - gPlayers[0]->position.y) > 0x200)
        goto zero;
    if (actor->base.tracking.distance < actor->base.tracking.limit)
        goto one;
zero:
    return 0;
one: {
    u32 mask = 0x80;
    u16 *flags = &actor->base.reactionFlags;
    mask |= *flags;
    *flags = mask;
}
    {
        u32 mask = 0x10;
        mask |= tracking->flags;
        tracking->flags = mask;
    }
    return 1;
}
