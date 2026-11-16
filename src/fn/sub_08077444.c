#include "scene_actor_placement.h"
#include "scene_actor_status.h"
#include "scene_actor_terrain_reaction.h"
static inline void SceneActorPlacement_ResetControl(struct ChaseActor *actor,
                                                    struct SceneActorRegistryControl *control)
{
    if (ActorStatus_BehaviorFlags(actor, 0x200000)) {
        ActorStatus_ClearInteraction(actor, control);
    }
}
static inline void SceneActorPlacement_ClearColliderFlags(struct ContactCollider *collider,
                                                          u16 flags)
{
    u32 value = collider->flags;
    u32 mask = (u16)~flags;
    mask &= value;
    collider->flags = mask;
}
void SceneActorPlacement_ResetToPathPoint(struct ChaseActor *object, u32 pathId, u32 pointId,
                                          u32 direction)
{
    struct ChaseActor *actor = (void *)object;
    struct SceneActorRegistryControlGroup *group = &actor->group.common;
    struct SceneActorBody *body = &actor->base.body;
    u32 *flags;
    u32 enabled = 1;
    flags = &actor->base.flags2CC;
    if (*flags & enabled) {
        struct ContactPosition *position;
        SceneActorChase_ClearWordFlags(flags, 3);
        actor->group.homeRoute.currentPoint = actor->group.homeRoute.pointId;
        position = &actor->base.body.position;
        Path_GetPointPositionById((u16 *)position, pathId, pointId);
        actor->base.terrainCell = SceneActorTerrain_GetCellIndex(position);
        body->position.y =
            SceneActorTerrain_GetHeight(actor->base.terrainCell, &actor->base.body.position);
        if (!actor->base.drawableKind)
            ((struct SceneActorRegistrySequenceDrawable *)actor->base.drawable)->position =
                body->position;
        else
            ((struct SceneActorRegistryTileDrawable *)actor->base.drawable)->position =
                body->position;
        body->direction = direction;
        SceneActorPlacement_ResetControl(actor, &group->control);
        SceneActorPlacement_ClearColliderFlags(&actor->base.colliderA, 4);
        ActorStatus_SetDrawableFlags(actor, 2);
        ActorStatus_ClearDrawableFlags(actor, 1);
        SceneActorTerrain_EnterState(actor, 3, 0, 0);
        SceneActorTerrain_InstallControl(actor, actor->base.onHidden, 0xC);
    }
}
