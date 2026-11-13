#ifndef GUARD_PLAYER_CUE_COMMANDS_H

#define GUARD_PLAYER_CUE_COMMANDS_H

#include "player.h"

#include "engine.h"

#include "contact_position.h"

void *ActorAttachment_UpdateFromDescriptor(void *, void *, u32, u32, void *, s32, s32, s32);

void Player_StopActionSounds(struct Player *);

void Player_RestoreCameraFollow(struct Player *);

void Player_AddControlFlags(struct Player *, u32);

void ActorSequence_Begin(struct Player *, s32, s32);

void PlayerCue_UpdateHidden(struct Player *);

void Player_UpdateFacingSide(struct Player *);

void Player_UpdateTriggerAnimation(struct Player *);

void Player_TurnTowardTargetFacing(struct Player *);

void PlayerCue_UpdateWalkTarget(struct Player *);

void PlayerCue_UpdateSideWalkTarget(struct Player *);

void Player_AdvanceScriptDestination(struct Player *);

void PlayerCue_UpdateMoveDestination(struct Player *);

void Player_UpdateSongCueAnimation(struct Player *);

void Player_UpdateTimedCueAnimation(struct Player *);

void PlayerCue_UpdateSideDeparture(struct Player *);

void PlayerCue_UpdateDeparture(struct Player *);

void PlayerCue_UpdateSideArrival(struct Player *);

void Player_UpdateTimedDirectionAction(struct Player *);

void PlayerCue_UpdateRecoilSlide(struct Player *);

void PlayerCue_UpdateSideRecoil(struct Player *);

void Player_UpdateRecoilAction(struct Player *);

void PlayerCue_UpdateKnockdownRecovery(struct Player *);

void PlayerCue_UpdateRecoilRecovery(struct Player *);

s32 ActorVariant_GetTableValue(u32, s32);

void PlayerCue_UpdateMotionFade(struct Player *);

void PlayerCue_UpdateSideFade(struct Player *);

void PlayerCue_UpdateAlternateFade(struct Player *);

void PlayerCue_UpdateScaleTransition(struct Player *);

void Player_UpdateFourPhaseAction(struct Player *);

void PlayerCue_UpdateAimHoldA(struct Player *);

void PlayerCue_UpdateAimHoldB(struct Player *);

void Player_UpdateSixPhaseAction(struct Player *);

void MovingSprite_Create_082375FC(struct SceneVec *, s32, s32, s32, s32, s32, s32);

s32 Player_SetAnimation(struct Player *, s32, s32, s32);

void PlayerCue_UpdateAim(struct Player *);

void PlayerCue_UpdateTurningPalette(struct Player *);

void PlayerCue_UpdateRunTo(struct Player *);

void PlayerCue_UpdateRunOverlay(struct Player *);

void Player_UpdateThreePhaseBaseAnimationAction(struct Player *);

void Player_UpdateTransitionAnimation(struct Player *);

void PlayerCue_UpdateStandDiagonal(struct Player *);

void PlayerCue_UpdateStandDiagonalB(struct Player *);

void PlayerCue_UpdateFaceSide(struct Player *);

void Player_UpdateAlternateFormCue(struct Player *);

void Player_UpdateFormChangeCue(struct Player *);

void PlayerCue_UpdateFormChange(struct Player *);

void PlayerCue_UpdateTargetDash(struct Player *);

void PlayerCue_UpdateSpiralMotion(struct Player *);

void PlayerCue_UpdateSpiralRecovery(struct Player *);

void Player_UpdateStateMotionCue(struct Player *);

s32 EffectGroup_Spawn(void *, s32, s32, struct SceneVec *);

void Player_EmitQueuedEffects(struct Player *);

struct PlayerPendingNotification {
    void *receiver;
    u32 flags;
    void (*callback)(void *, s32, s32);
};

s32 Player_EnableStatGaugeFromScript(void);
s32 Player_EnableStatGaugeAndHistoryFromScript(void);
s32 Player_RestrictActionRequestsFromScript(void);
void Player_EnterKindTwo(struct Player *player);
void PlayerCue_OverlaySet(struct Player *player, s32 parameter);
void PlayerCue_OverlayOn(struct Player *player, s32 parameter);
void PlayerCue_OverlayOff(struct Player *player);
void PlayerCue_SetPosition(struct Player *player, const struct ContactPosition *position);
void PlayerCue_Hide(struct Player *player);
void PlayerCue_Stand(struct Player *player, s32 facing);
void PlayerCue_Face(struct Player *player, u32 targetFacing, u32 parameter);
void PlayerCue_Idle(struct Player *player, s32 facing);
void PlayerCue_StandVariant2(struct Player *player, s32 facing, u32 parameter);
void PlayerCue_StandVariant3(struct Player *player, s32 facing, u32 parameter);
void PlayerCue_WalkTo(struct Player *player, const struct ContactPosition *destination,
                      u32 parameter, s32 targetFacing);
void PlayerCue_MoveTo(struct Player *player, const struct ContactPosition *position, s32 facing,
                      s32 animated, u32 parameter);
void PlayerCue_Motion7Front(struct Player *player, u32 variant, u32 parameter);
void PlayerCue_Motion7(struct Player *player, u32 variant, u32 parameter);
void PlayerCue_Motion7End(struct Player *player, u32 parameter);
void PlayerCue_Kneel(struct Player *player, s32 facing, u32 parameter);
void PlayerCue_KneelGasp(struct Player *player, s32 facing);
void PlayerCue_KneelDown(struct Player *player, s32 facing);
void PlayerCue_GetUp(struct Player *player, s32 facing, u32 parameter);
void PlayerCue_BeginMode27(struct Player *player, s32 facing, s32 variant, u32 parameter);
void PlayerCue_BeginMode28(struct Player *player, s32 facing, s32 variant, u32 parameter);
void PlayerCue_Motion29(struct Player *player, s32 facing, u32 speed, u32 parameter);
void PlayerCue_Motion30(struct Player *player, s32 facing, u32 speed, u32 variant, u32 parameter);
void PlayerCue_Motion30End(struct Player *player, u32 parameter);
void PlayerCue_Motion29B(struct Player *player, s32 facing, u32 speed, u32 parameter);
void PlayerCue_BeginOddDirection(struct Player *player, s32 direction);
void PlayerCue_Motion7Turn(struct Player *player, s32 facing, u32 parameter);
void PlayerCue_Motion7Variant7(struct Player *player, s32 facing, u32 parameter);
void PlayerCue_Motion7Variant3(struct Player *player, s32 facing, u32 parameter);
void PlayerCue_AimDraw(struct Player *player, s32 facing, u32 parameter);
void PlayerCue_AimDrawVariant2(struct Player *player, s32 facing, u32 parameter);
void PlayerCue_AimSteady(struct Player *player, s32 facing, u32 parameter);
void PlayerCue_AimSteadyVariant2(struct Player *player, s32 facing, u32 parameter);
void PlayerCue_SetAimPose(struct Player *player, s32 facing, u32 parameter);
void PlayerCue_SetFiringPose(struct Player *player, s32 facing, u32 parameter);
void PlayerCue_Aim(struct Player *player, s32 facing, u32 parameter);
void PlayerCue_Shoot(struct Player *player, s32 projectileParameter, u32 parameter);
void PlayerCue_AimHold(struct Player *player, s32 facing, u32 phase, u32 parameter);
void PlayerCue_AimEnd(struct Player *player, u32 parameter);
void PlayerCue_BeginModeSeven(struct Player *player, s32 mode);
void PlayerCue_RunTo(struct Player *player, const struct ContactPosition *destination,
                     u32 targetFacing, u32 parameter);
void PlayerCue_Run(struct Player *player, s32 facing);
void PlayerCue_Motion17(struct Player *player, s32 unused);
void PlayerCue_Motion17Variant2(struct Player *player, s32 unused);
void PlayerCue_Windblown(struct Player *player, s32 backSide, u32 parameter);
void PlayerCue_WindblownVariant3(struct Player *player, s32 backSide, u32 parameter);
void PlayerCue_WindblownVariant4(struct Player *player, s32 backSide, u32 parameter);
void PlayerCue_WindblownVariant2(struct Player *player, s32 backSide, u32 parameter);
void PlayerCue_StandDiagonal(struct Player *player, s32 facing, u32 parameter);
void PlayerCue_StandDiagonalB(struct Player *player, s32 facing, u32 parameter);
void PlayerCue_FaceSide(struct Player *player, u32 backSide);
s32 PlayerCue_Motion11(struct Player *player, u32 parameter);
s32 PlayerCue_Motion10(struct Player *player, u32 parameter);
s32 PlayerCue_FormMotion(struct Player *player, u32 parameter);
u32 PlayerCue_Motion38To(struct Player *player, const struct SceneVec *destination, u32 parameter);
u32 PlayerCue_StateMotion(struct Player *player, u32 parameter);
void PlayerCue_ShakeEffect(struct Player *player, s32 effectKind);
void PlayerCue_Refresh(struct Player *player);
void Player_ConfigureAttachmentEffect(struct Player *player, u32 action, s32 value, s32 arg);
void PlayerCue_ReleaseControl(struct Player *player);
#endif
