#ifndef GUARD_PLAYER_CUE_ACTIONS_H
#define GUARD_PLAYER_CUE_ACTIONS_H
#include "player.h"
#include "contact_swarm.h"
#include "display_sequence.h"
#include "script_utility.h"
#include "scene_event_dispatch.h"

void ActorSequence_Begin(struct Player *, s32, s32);
void Player_UpdateTriggerAnimation(struct Player *player);
void ScriptActor_CallCompletionScript(struct Player *player);
void PlayerCue_AcknowledgePendingTrigger(struct Player *player);
void Player_AddControlFlags(struct Player *, u32);
void SceneActor_SpawnModeParticles(struct Player *);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(struct Player *, s32, s32, s32);
void PlayerCue_EnterStandUpdate(struct Player *player);
void PlayerCue_CompleteAction(struct Player *player);
void Player_UpdateFacingSide(struct Player *);
void PlayerCue_UpdateHidden(struct Player *player);
s32 Player_GetMovementRateScale(s32);
void Player_ConstrainTetherTarget(void *, s32, s32, u32);
void Camera_SnapToWithoutCompletion(struct SceneVec *);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void DirectionalSpriteEffect_LaunchFromFacing(void *, u32, struct SceneVec *);
void ActorGlyphTrail_Create(struct Player *, s32, s32);
void Player_SetTetherMode(struct Player *, s32, s32);
void Player_RestartAnimationFromSetup(struct Player *player);
s32 Player_ConsumeStateEntry(void *);
void Player_SetActionPhase(void *, s32);
void Player_InitVisualResources(void *);
void SpiralTrail_CreateWithParticles(struct Player *, s32, void *, s32, s32);
void Player_RestartAnimationFromResource(struct Player *player);
void PlayerCue_RestartAlternateAnimation(struct Player *player);
void Player_SetActionOrigin(void *);
void ConvergingEffects_EmitNearParticle(void *, void *);
void Hud_ClearTimer(void);
void ConvergingEffects_EmitFarParticle(void *, void *);
u32 ParticlePool_Create(void);
void Player_SetPendingCallback(void *, void *);
void Player_OnTargetMotionExit(void);
void Player_EmitQueuedEffects(void *);
void Player_NotifyActionCategories(void *);
void Player_SetFacingFromAngle(void *, s32);
void ParticlePool_EmitTriple(u32, void *, u32, u32);
void SceneMovingParticle_Create(void *, void *, s32, s32);
void PlayerCue_InitAttackCollider(void *, void *, u32, u32);
s32 Div(s32, s32);

union PlayerCueReference {
    void *actor;
    struct Player *player;
};

union TargetDashKeyframeSelection {
    u32 raw;
    u16 sequence;
};
#endif
