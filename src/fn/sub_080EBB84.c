#include "global.h"
#include "scene_actor_reaction.h"
struct SceneActorTerrainCollisionView;
void SceneActorTerrain_DisableCollision(struct SceneActorTerrainCollisionView *);
static inline u8 TakeReset(struct SceneActorRegistryResetView *actor)
{
    u8 *flag = &actor->field2B1;
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
}
static inline void ResetState(struct SceneActorRegistryResetView *actor, u32 state)
{
    u8 *field = &actor->state2A8;
    u32 zero = 0;
    u32 offset;
    *field = state;
    offset = 0x2A9;
    *((u8 *)actor + offset) = zero;
    offset++;
    *((u8 *)actor + offset) = zero;
    offset += 2;
    *(u32 *)((u8 *)actor + offset) = zero;
    actor->field2B1 = 1;
}
void SceneActorReaction_BeginHiddenCollisionless(struct SceneActorRegistryResetView *actor)
{
    if (TakeReset(actor)) {
        u32 mask = 1;
        if (!actor->drawableKind)
            *(u32 *)actor->drawable |= mask;
        else {
            struct SceneActorAnimationTile *tile = (void *)((u8 *)actor->drawable + 0x20);
            tile->flags |= mask;
        }
        ResetState(actor, 3);
        SceneActorTerrain_DisableCollision((void *)actor);
    }
    actor->counter2AC++;
}
