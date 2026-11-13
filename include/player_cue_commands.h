#ifndef GUARD_PLAYER_CUE_COMMANDS_H
#define GUARD_PLAYER_CUE_COMMANDS_H

#include "player.h"
#include "engine.h"
#include "contact_position.h"

void *ActorAttachment_UpdateFromDescriptor(void *, void *, u32, u32, void *, s32, s32, s32);
void Player_StopActionSounds(struct Player *);
void Player_RestoreCameraFollow(struct Player *);
void Player_AddControlFlags(struct Player *, u32);
void Player_EnterKindTwo(struct Player *player);
void ActorSequence_Begin(struct Player *, s32, s32);
void PlayerCue_UpdateHidden(struct Player *);
void Player_UpdateFacingSide(struct Player *);
void Player_UpdateTriggerAnimation(struct Player *);
void Player_TurnTowardTargetFacing(struct Player *);
void PlayerCue_UpdateWalkTarget(struct Player *);
void PlayerCue_UpdateSideWalkTarget(struct Player *);
void Player_AdvanceScriptDestination(struct Player *);
void PlayerCue_UpdateMoveDestination(struct Player *);
void PlayerCue_Stand(struct Player *player, s32 v);
void Player_UpdateSongCueAnimation(struct Player *);
void Player_UpdateTimedCueAnimation(struct Player *);
void PlayerCue_Motion7Turn(struct Player *player, s32 dir, u32 c);
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
#endif
