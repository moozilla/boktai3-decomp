#include "scene_actor_terrain_reaction.h"
#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
// confidence=low: hides the drawable and moves the actor above the active field.
#define SceneActorControl_HideOffscreen sub_081304C8
static inline u8 TakeReset(struct ChaseActor *a)
{
    if (a->base.field2B1) {
        a->base.field2B1 = 0;
        return 1;
    }
    return 0;
}
static inline void SetFlags(struct ChaseActor *actor, u32 mask)
{
    u16 value = mask;
    value |= actor->base.motionFlags;
    actor->base.motionFlags = value;
}
static inline void SceneActorControl_SetDrawableFlags(u32 bits, struct ChaseActor *actor)
{
    if (actor->base.drawableKind == 0) {
        struct DisplaySequenceSprite *sprite = actor->base.drawable;
        sprite->flags |= bits;
    } else {
        struct SceneActorAnimationTileDrawable *drawable = actor->base.drawable;
        struct SceneInputWidget *tile = &drawable->tile;
        tile->flags |= bits;
    }
}

void SceneActorControl_HideOffscreen(struct ChaseActor *actor)
{
    if (TakeReset(actor)) {
        struct ContactPosition *position;
        if (actor->base.drawableKind == 0)
            position =
                &((struct SceneActorRegistrySequenceDrawable *)actor->base.drawable)->position;
        else
            position = &((struct SceneActorRegistryTileDrawable *)actor->base.drawable)->position;
        position->y = 30000;
    }
    actor->base.body.position.x = 0;
    actor->base.body.position.y = 30000;
    actor->base.body.position.z = 0;
    SceneActorControl_SetDrawableFlags(1, actor);
    SceneActorTerrain_EnterState(actor, 3, 0, 0);
    SceneActorTerrain_InstallControl(actor, SceneActorControl_OnIdle, 16);
    SceneActorTerrain_DisableCollision(actor);
    SetFlags(actor, 0x40);
    actor->base.counter2AC++;
}
