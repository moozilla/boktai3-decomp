#ifndef GUARD_ACTOR_PHASE_H
#define GUARD_ACTOR_PHASE_H
#include "engine.h"
#include "scene_actor_state.h"
#include "actor_animation_controller.h"
/* Axe phase workspace; names describe the trail and relocation consumers. */
struct ActorPhaseContext {
    u8 unknown0[0x888];
    u8 visibleAnimation[0x80];
    u8 flickerAnimation[0x11C];
    struct ContactPosition trailOrigin;
    u8 unknownA2C[8];
    u8 trailAngle, trailMode, angularSpeed, unknownA37;
    struct ActorSprite trailSprite;
    u8 unknownA5C[0xCA8 - 0xA5C];
    u8 initialAngle, unknownCA9[2], animationState;
    u8 shadow[0xAC];
    u8 unknownD58[0xE40 - 0xD58];
    u8 relocationDelay;
    u8 unknownE41[11];
    u32 flags;
};
void ActorPhase_UpdateAcceleratingTrail(struct SceneActorRegistryResetView *, s32);
void ActorPhase_UpdateReverseFlicker(struct SceneActorRegistryResetView *, s32);
void ActorPhase_UpdateReverseFlickerExit(struct SceneActorRegistryResetView *, s32);
void ActorPhase_UpdateSpriteCollapse(struct SceneActorRegistryResetView *);
void ActorPhase_WaitBeforeRelocation(struct SceneActorRegistryResetView *, s32);
void ActorPhase_UpdateSpriteRestore(struct SceneActorRegistryResetView *);
void ActorPhase_EmitTrailSprite(struct SceneActorRegistryResetView *, void *, u32);
void Axe_UpdateTrailColliders(struct SceneActorRegistryResetView *);
void Axe_UpdateAttackColliderPositions(struct SceneActorRegistryResetView *);
void Axe_ReleaseAuxiliarySprite(struct SceneActorRegistryResetView *);
void SceneActorControl_ResetRotation(void);
u8 ActorPhase_UpdateReverseFlickerTimer(u32 *, s32, s32, s32);
void Axe_SetAuxiliarySpriteParameters(struct SceneActorRegistryResetView *, void *, u32);
u8 Sprite_BlinkUntilExpired(void *, s32, s32, s32);
void ActorPhase_SelectDistantWaypoint(struct SceneActorRegistryResetView *, void *, void *, s32);
#endif
