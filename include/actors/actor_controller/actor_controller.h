#ifndef GUARD_ACTOR_CONTROLLER_H
#define GUARD_ACTOR_CONTROLLER_H
#include "actors/scene_combat_actor/scene_combat_actor.h"
#include "engine/shared/engine.h"
#include "engine/shared/ram.h"
s32 DirectionalActor_TryBeginState(struct SceneCombatActor *, u32);
u32 ActorController_StartTurn(struct SceneCombatActor *, u32);
u32 ActorController_StartModeTwoTurn(struct SceneCombatActor *, u32);
u32 ActorController_StartModeThreeTurn(struct SceneCombatActor *, u32);
u32 ActorController_StartModeOneTurn(struct SceneCombatActor *, u32);
u32 ActorController_IsNearTarget(struct SceneCombatActor *);
void ActorController_UpdateTargetMovement(struct SceneCombatActor *, s32, s32);
s32 Terrain_ApplyBlocked(void *, void *, struct ContactPosition *);
u32 NavMesh_ConsumeProbeEvent(u16 *);
u32 NavMesh_SteerCardinal(void *, void *, u32, u32);
void Armor_UpdatePositiveTurn(struct SceneCombatActor *, s32);
void Armor_UpdateNegativeTurn(struct SceneCombatActor *, s32);
void Armor_SetAnimationFromStart(struct SceneCombatActor *, const void *);
void Armor_StartFullPaletteRestoreDelay(struct SceneCombatActor *);
void ArmorActor_UpdateFixedSpeedMotion(struct SceneCombatActor *);
void ActorController_UpdateResetMotionTransition(struct SceneCombatActor *, s32);
union ActorControllerPosition {
    struct ContactPosition position;
    s16 components[4];
};
void ActorController_UpdateMovingAttackCollider(struct SceneCombatActor *);
static inline void ActorController_ClearColliderFlags(struct ContactCollider *collider, u32 flags)
{
    collider->flags &= ~flags;
}

static inline void ActorController_SetColliderFlags(struct ContactCollider *collider, u32 flags)
{
    collider->flags |= flags;
}

/* Collider zero is the attack collider. These profiles leave it inactive. */
static inline void ActorController_SelectDefenseColliders(struct SceneCombatActor *actor)
{
    ActorController_ClearColliderFlags(&actor->colliderGroup.colliders[2], 4);
    ActorController_ClearColliderFlags(&actor->colliderGroup.colliders[1], 4);
    ActorController_ClearColliderFlags(&actor->colliderGroup.colliders[3], 4);
    ActorController_SetColliderFlags(&actor->colliderGroup.colliders[0], 4);
}

static inline void ActorController_SelectPairedColliders(struct SceneCombatActor *actor)
{
    ActorController_SetColliderFlags(&actor->colliderGroup.colliders[2], 4);
    ActorController_ClearColliderFlags(&actor->colliderGroup.colliders[1], 4);
    ActorController_ClearColliderFlags(&actor->colliderGroup.colliders[3], 4);
    ActorController_SetColliderFlags(&actor->colliderGroup.colliders[0], 4);
}

static inline void ActorController_ClearMotionFlags(struct ActorControllerMotion *motion, u32 flags)
{
    motion->flags &= ~flags;
}

static inline s32 ActorController_TruncateMotion(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}

static inline void ActorController_SetMotion(struct ActorControllerVelocity *velocity, s32 angle,
                                             s32 speed)
{
    velocity->x = ActorController_TruncateMotion(gUnk_086149C4[(angle + 64) & 255] * speed);
    velocity->y = 0;
    velocity->z = ActorController_TruncateMotion(gUnk_086149C4[angle] * speed);
}

#endif
