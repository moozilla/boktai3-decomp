#include "global.h"
#include "scene_actor_reaction.h"
struct RoomActor;
void SceneActorTerrain_DisableCollision(struct RoomActor *);
static inline u8 TakeReset(struct ChaseActor *actor)
{
    u8 *flag = &actor->base.field2B1;
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
}
static inline void ResetState(struct ChaseActor *actor, u32 state)
{
    u8 *field = &actor->base.state2A8;
    u32 zero = 0;
    u32 offset;
    *field = state;
    offset = 0x2A9;
    *((u8 *)actor + offset) = zero;
    offset++;
    *((u8 *)actor + offset) = zero;
    offset += 2;
    *(u32 *)((u8 *)actor + offset) = zero;
    actor->base.field2B1 = 1;
}
void SceneActorReaction_BeginHiddenCollisionless(struct ChaseActor *actor)
{
    if (TakeReset(actor)) {
        u32 mask = 1;
        if (!actor->base.drawableKind)
            *(u32 *)actor->base.drawable |= mask;
        else {
            struct SceneActorAnimationTile *tile = (void *)((u8 *)actor->base.drawable + 0x20);
            tile->flags |= mask;
        }
        ResetState(actor, 3);
        SceneActorTerrain_DisableCollision((void *)actor);
    }
    actor->base.counter2AC++;
}
