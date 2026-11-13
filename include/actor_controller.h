#ifndef GUARD_ACTOR_CONTROLLER_H
#define GUARD_ACTOR_CONTROLLER_H
#include "scene_combat_actor.h"
#include "engine.h"
#include "ram.h"
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
void ActorController_UpdateResetMotionTransition(struct SceneCombatActor *, s32);
#endif
