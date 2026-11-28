#include "scene/actor/scene_actor_behavior.h"
#include "scene/motion/scene_actor_chase_shared.h"
/* SceneActorBehavior_TestDefeated: Records defeat when counter304 is positive and updates actor and gate flags accordingly */

void SceneActorBehavior_RecordDefeat(struct RoomActor *);

static inline u32 OrV(u16 m, u16 *f)
{
    return m | *f;
}

u32 SceneActorBehavior_TestDefeated(struct RoomActor *actor)
{
    struct SceneActorChaseTracking *tracking;
    if (actor->counter304 > 0) {
        u8 mask;
        s32 flags;
        SceneActorBehavior_RecordDefeat(actor);
        flags = OrV(0x80, &actor->reactionFlags) & ~0x100;
        actor->reactionFlags = flags;
        tracking = &actor->tracking;
        mask = 0x10;
        tracking->flags |= mask;
        return 1;
    }
    tracking = &actor->tracking;
    if (SceneActorChase_TrackingFlags(tracking, 2)) {
        s32 mask = ~0x10;
        tracking->flags &= mask;
    }
    return 0;
}
