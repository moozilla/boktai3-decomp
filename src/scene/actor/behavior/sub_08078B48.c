#include "scene/actor/scene_actor_behavior.h"
#include "scene/actor/scene_actor_chase_shared.h"
/* SceneActorBehavior_UpdateDefeatState: Records defeat when counter304 is positive and updates actor and gate flags accordingly */
#define SceneActorBehavior_UpdateDefeatState sub_08078B48
#define SceneActorBehavior_TestDefeated      sub_08078B48

void SceneActorBehavior_RecordDefeat(struct RoomActor *);

static inline u32 OrV(u16 m, u16 *f)
{
    return m | *f;
}

u32 SceneActorBehavior_TestDefeated(struct RoomActor *actor)
{
    struct SceneActorChaseTracking *sprite;
    if (actor->counter304 > 0) {
        u8 m;
        SceneActorBehavior_RecordDefeat(actor);
        {
            s32 v = OrV(0x80, &actor->reactionFlags) & ~0x100;
            *&actor->reactionFlags = v;
        }
        sprite = &actor->tracking;
        m = 0x10;
        sprite->flags |= m;
        return 1;
    }
    sprite = &actor->tracking;
    if (SceneActorChase_TrackingFlags(sprite, 2)) {
        s32 m = ~0x10;
        sprite->flags &= m;
    }
    return 0;
}
